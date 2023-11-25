// Datastructures.cc
//
// Student name: Taisto Tammilehto

#include "datastructures.hh"
#include <random>
#include <unordered_set>
#include <cmath>
#include <vector>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
}

Datastructures::~Datastructures()
{
}

// Returns the number of affiliations currently stored
unsigned int Datastructures::get_affiliation_count() {
    return affiliations.size();
}

// Clears all stored data, resetting the data structure to its initial state
void Datastructures::clear_all() {
    affiliations.clear();
    publications.clear();
    affiliations_publications.clear();
    coord_to_affiliation.clear();

    // Clear the sorted sets
    sorted_affiliations_by_name.clear();
    sorted_affiliations_by_distance.clear();

    // Reset the flags
    affiliations_sorted_by_name = true;
    affiliations_sorted_by_distance = true;
}

// Retrieves a list of all affiliations in no particular order
std::vector<AffiliationID> Datastructures::get_all_affiliations() {
    std::vector<AffiliationID> all_affiliations;
    for (const auto& entry : affiliations) {
        all_affiliations.push_back(entry.first);
    }
    return all_affiliations;
}

// Adds a new affiliation, returns true if successful or false if the affiliation already exists
bool Datastructures::add_affiliation(AffiliationID id, Name const& name, Coord xy) {
    if (affiliations.find(id) == affiliations.end()) {
        affiliations[id] = std::make_tuple(name, xy);

        // Update the sorted sets
        sorted_affiliations_by_name.insert({name, id});
        sorted_affiliations_by_distance.insert({calculate_distance_from_origin(xy), id});

        return true;
    }
    return false;
}

// Retrieves the name of a specified affiliation
Name Datastructures::get_affiliation_name(AffiliationID id) {
    auto it = affiliations.find(id);
    if (it != affiliations.end()) {
        return std::get<0>(it->second);
    }
    return NO_NAME;
}

// Retrieves the coordinates of a specified affiliation
Coord Datastructures::get_affiliation_coord(AffiliationID id) {
    auto it = affiliations.find(id);
    if (it != affiliations.end()) {
        return std::get<1>(it->second);
    }
    return NO_COORD;
}

// Returns a list of affiliations sorted alphabetically by their names
std::vector<AffiliationID> Datastructures::get_affiliations_alphabetically() {
    update_sorted_affiliations_by_name();
    std::vector<AffiliationID> result;
    for (const auto& [name, id] : sorted_affiliations_by_name) {
        result.push_back(id);
    }
    return result;
}

// Returns a list of affiliations sorted by increasing distance from the origin
std::vector<AffiliationID> Datastructures::get_affiliations_distance_increasing() {
    std::vector<AffiliationID> result;
    for (const auto& [distance, id] : sorted_affiliations_by_distance) {
        result.push_back(id);
    }
    return result;
}

void Datastructures::update_sorted_affiliations_by_name() {
    if (!affiliations_sorted_by_name) {
        sorted_affiliations_by_name.clear();
        for (const auto& [id, data] : affiliations) {
            sorted_affiliations_by_name.emplace(std::get<0>(data), id);
        }
        affiliations_sorted_by_name = true;
    }
}

void Datastructures::update_sorted_affiliations_by_distance() {
    if (!affiliations_sorted_by_distance) {
        sorted_affiliations_by_distance.clear();
        for (const auto& [id, data] : affiliations) {
            sorted_affiliations_by_distance.emplace(calculate_distance_from_origin(std::get<1>(data)), id);
        }
        affiliations_sorted_by_distance = true;
    }
}

Distance Datastructures::calculate_distance_from_origin(Coord coord) {
    return coord.x * coord.x + coord.y * coord.y;
}

// Changes the coordinates of a specified affiliation
bool Datastructures::change_affiliation_coord(AffiliationID id, Coord newcoord) {
    auto it = affiliations.find(id);
    if (it != affiliations.end()) {
        // Update the coordinates in the affiliations map
        Coord old_coord = std::get<1>(it->second);
        std::get<1>(it->second) = newcoord;

        // Update the sorted_affiliations_by_distance set
        Distance old_distance = calculate_distance_from_origin(old_coord);
        Distance new_distance = calculate_distance_from_origin(newcoord);
        sorted_affiliations_by_distance.erase({old_distance, id});
        sorted_affiliations_by_distance.insert({new_distance, id});

        // Update coord_to_affiliation map
        auto& old_vec = coord_to_affiliation[old_coord];
        auto find_iter = std::find(old_vec.begin(), old_vec.end(), id);
        if (find_iter != old_vec.end()) {
            old_vec.erase(find_iter);
        }
        coord_to_affiliation[newcoord].push_back(id);

        return true;
    }
    return false;
}

// Finds and returns the ID of an affiliation at a specific coordinate
AffiliationID Datastructures::find_affiliation_with_coord(Coord xy) {
    auto it = coord_to_affiliation.find(xy);
    if (it != coord_to_affiliation.end() && !it->second.empty()) {
        return it->second.front();
    }
    return NO_AFFILIATION;
}

// Adds a new publication, returns true if successful or false if the publication already exists
bool Datastructures::add_publication(PublicationID id, const Name& name, Year year, const std::vector<AffiliationID>& affs) {
    if (publications.find(id) != publications.end()) {
        return false;
    }

    std::vector<AffiliationID> valid_affiliations;
    for (const auto& aff_id : affs) {
        if (affiliations.find(aff_id) != affiliations.end()) {
            valid_affiliations.push_back(aff_id);
            affiliations_publications[aff_id].push_back(id);
        }
    }

    publications[id] = {name, year, valid_affiliations, {}};
    return true;
}

// Returns a list of all publications
std::vector<PublicationID> Datastructures::all_publications() {
    std::vector<PublicationID> result;
    for (const auto& entry : publications) {
        result.push_back(entry.first);
    }
    return result;
}

// Retrieves the name of a specified publication
Name Datastructures::get_publication_name(PublicationID id)
{
    if (publications.find(id) != publications.end()) {
        return publications[id].name;
    }

    return NO_NAME;
}

// Retrieves the publication year of a specified publication
Year Datastructures::get_publication_year(PublicationID id)
{
    if (publications.find(id) != publications.end()) {
        return publications[id].year;
    }

    return NO_YEAR;
}

// Retrieves a list of affiliations associated with a specified publication
std::vector<AffiliationID> Datastructures::get_affiliations(PublicationID id)
{
    if (publications.find(id) != publications.end()) {
        return publications[id].affiliations;
    }

    return {};
}

// Adds a reference from one publication (child) to another (parent)
bool Datastructures::add_reference(PublicationID child, PublicationID parent) {
    auto child_it = publications.find(child);
    auto parent_it = publications.find(parent);
    if (child_it != publications.end() && parent_it != publications.end()) {
        // Set the parent of the child publication
        child_it->second.parent = parent;
        // Add the child to the parent's list of references
        parent_it->second.references.push_back(child);
        // Update reverse_references
        reverse_references[child].push_back(parent);
        return true;
    }
    return false;
}

// Retrieves a list of publications that a specified publication directly references
std::vector<PublicationID> Datastructures::get_direct_references(PublicationID id)
{
    auto it = publications.find(id);
    if (it != publications.end()) {
        return it->second.references;
    }
    return {}; // Return an empty vector if the publication does not exist
}

// Associates an affiliation with a specified publication
bool Datastructures::add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid)
{
    if (publications.find(publicationid) != publications.end() && affiliations.find(affiliationid) != affiliations.end()) {
        publications[publicationid].affiliations.push_back(affiliationid);

        // Check if the affiliation already has publications
        if (affiliations_publications.find(affiliationid) == affiliations_publications.end()) {
            // Initialize with the first publication
            affiliations_publications[affiliationid] = {publicationid};
        } else {
            // Add to existing publications
            affiliations_publications[affiliationid].push_back(publicationid);
        }

        return true;
    }

    return false;
}

// Retrieves a list of publications associated with a specified affiliation
std::vector<PublicationID> Datastructures::get_publications(AffiliationID id)
{
    // Check if the affiliation exists
    if (affiliations.find(id) == affiliations.end()) {
        // The affiliation does not exist
        return {NO_PUBLICATION};
    }

    // The affiliation exists, check if it has any publications
    auto it = affiliations_publications.find(id);
    if (it == affiliations_publications.end()) {
        // The affiliation exists but has no publications
        return {};
    }

    // Return the list of publications for the affiliation
    return it->second;
}

// Retrieves the parent publication of a specified publication
PublicationID Datastructures::get_parent(PublicationID id) {
    auto it = publications.find(id);
    if (it != publications.end()) {
        return it->second.parent;
    }
    return NO_PUBLICATION;
}

// Retrieves a list of publications after a specified year associated with a specific affiliation
std::vector<std::pair<Year, PublicationID>> Datastructures::get_publications_after(AffiliationID affiliationid, Year year)
{
    std::vector<std::pair<Year, PublicationID>> result;

    auto it = affiliations_publications.find(affiliationid);
    if (it != affiliations_publications.end()) {
        for (PublicationID id : it->second) {
            if (publications[id].year >= year) { // Changed from > to >=
                result.emplace_back(publications[id].year, id);
            }
        }
    }

    // Sort by year and then by ID
    std::sort(result.begin(), result.end());
    return result;
}

// Retrieves a chain of publications that reference a specific publication
std::vector<PublicationID> Datastructures::get_referenced_by_chain(PublicationID id) {
    if (publications.find(id) == publications.end()) {
        return {NO_PUBLICATION}; // Publication does not exist
    }

    std::vector<PublicationID> chain;
    std::unordered_set<PublicationID> visited;

    std::function<void(PublicationID)> dfs = [&](PublicationID current_id) {
        for (PublicationID ref_id : reverse_references[current_id]) {
            if (visited.insert(ref_id).second) {
                chain.push_back(ref_id);
                dfs(ref_id);
            }
        }
    };

    dfs(id);
    return chain;
}

// Retrieves all publications referenced by a specified publication
std::vector<PublicationID> Datastructures::get_all_references(PublicationID id)
{
    auto it = publications.find(id);
    if (it == publications.end()) {
        return {NO_PUBLICATION}; // Handle non-existing publication
    }

    std::unordered_set<PublicationID> visited;
    std::vector<PublicationID> result;
    std::function<void(PublicationID)> dfs = [&](PublicationID current_id) {
        for (PublicationID ref_id : publications[current_id].references) {
            if (visited.insert(ref_id).second) { // Avoid duplicates
                result.push_back(ref_id);
                dfs(ref_id);
            }
        }
    };

    dfs(id);

    return result;
}

std::vector<AffiliationID> Datastructures::get_affiliations_closest_to(Coord xy)
{
    std::vector<std::pair<Distance, AffiliationID>> distances;
    for (const auto& aff : affiliations) {
        Coord aff_coord = std::get<1>(aff.second);
        int dx = aff_coord.x - xy.x;
        int dy = aff_coord.y - xy.y;
        Distance dist = dx * dx + dy * dy;
        distances.emplace_back(dist, aff.first);
    }

    std::sort(distances.begin(), distances.end());

    std::vector<AffiliationID> closest;
    for (int i = 0; i < std::min(3, static_cast<int>(distances.size())); ++i) {
        closest.push_back(distances[i].second);
    }

    return closest;
}

bool Datastructures::remove_affiliation(AffiliationID id)
{
    auto aff_it = affiliations.find(id);
    if (aff_it == affiliations.end()) {
        return false;
    }

    for (auto& pub : publications) {
        pub.second.affiliations.erase(
            std::remove(pub.second.affiliations.begin(), pub.second.affiliations.end(), id),
            pub.second.affiliations.end());
    }

    affiliations.erase(aff_it);
    return true;
}

// Utility function to get the ancestors of a publication
std::vector<PublicationID> Datastructures::get_ancestors(PublicationID id) {
    std::vector<PublicationID> ancestors;
    while (id != NO_PUBLICATION) {
        ancestors.push_back(id);
        id = get_parent(id); // Assuming get_parent returns NO_PUBLICATION if parent doesn't exist
    }
    return ancestors;
}

// Function to find the closest common parent of two publications
PublicationID Datastructures::get_closest_common_parent(PublicationID id1, PublicationID id2) {
    std::vector<PublicationID> ancestors1 = get_ancestors(id1);
    std::vector<PublicationID> ancestors2 = get_ancestors(id2);

    // Reverse the ancestor vectors to start comparing from the root
    std::reverse(ancestors1.begin(), ancestors1.end());
    std::reverse(ancestors2.begin(), ancestors2.end());

    // Find the first common ancestor
    for (auto ancestor1 : ancestors1) {
        if (std::find(ancestors2.begin(), ancestors2.end(), ancestor1) != ancestors2.end()) {
            return ancestor1;
        }
    }

    return NO_PUBLICATION; // Return NO_PUBLICATION if there's no common ancestor
}

bool Datastructures::remove_publication(PublicationID publicationid)
{
    auto pub_it = publications.find(publicationid);
    if (pub_it == publications.end()) {
        return false; // Publication does not exist
    }

    // Remove publication from affiliations' publications list
    for (const auto& affiliation_id : pub_it->second.affiliations) {
        auto& pubs = affiliations_publications[affiliation_id];
        pubs.erase(std::remove(pubs.begin(), pubs.end(), publicationid), pubs.end());
    }

    // Remove the publication from reverse_references of its references
    for (auto& reference_id : pub_it->second.references) {
        if (publications.find(reference_id) != publications.end()) {
            auto& reverse_refs = reverse_references[reference_id];
            reverse_refs.erase(std::remove(reverse_refs.begin(), reverse_refs.end(), publicationid), reverse_refs.end());
        }
    }

    // Update references in other publications that reference this publication
    for (auto& [id, pub] : publications) {
        pub.references.erase(
            std::remove(pub.references.begin(), pub.references.end(), publicationid),
            pub.references.end());
    }

    // Remove the publication from reverse_references map
    reverse_references.erase(publicationid);

    // Finally, remove the publication
    publications.erase(pub_it);
    return true;
}
