// Datastructures.hh
//
// Student name: Taisto Tammilehto
// Student email: taisto.tammilehto@tuni.fi
// Student number: 151238789

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>
#include <set>

// Types for IDs
using AffiliationID = std::string;
using PublicationID = unsigned long long int;
using Name = std::string;
using Year = unsigned short int;
using Weight = int;
using Distance = int;

// Return values for cases where required thing was not found
AffiliationID const NO_AFFILIATION = "---";
PublicationID const NO_PUBLICATION = -1;
Name const NO_NAME = "!NO_NAME!";
Year const NO_YEAR = -1;
Weight const NO_WEIGHT = -1;

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// *********************OMA LISÄÄMÄ******************** HOX
struct PublicationInfo;

// Definition of the PublicationInfo struct
struct PublicationInfo
{
    Name name;
    Year year;
    std::vector<AffiliationID> affiliations;
    std::vector<PublicationID> references;
    PublicationID parent = NO_PUBLICATION;
};

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};


// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Returns the size of a hash map, which is an O(1) operation.
    unsigned int get_affiliation_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Clears all elements in hash maps, which takes linear time in the number of elements.
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Iterates over all elements in a hash map, which takes linear time.
    std::vector<AffiliationID> get_all_affiliations();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Inserts an element into a hash map, which is an average constant time operation.
    bool add_affiliation(AffiliationID id, Name const& name, Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses an element in a hash map.
    Name get_affiliation_name(AffiliationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses an element in a hash map.
    Coord get_affiliation_coord(AffiliationID id);

    // Estimate of performance: O(n * log(n))
    // Short rationale for estimate: Sorts affiliations based on names, which takes O(n log n) time.
    std::vector<AffiliationID> get_affiliations_alphabetically();

    // Estimate of performance: O(n * log(n))
    // Short rationale for estimate: Sorts affiliations based on distance, which takes O(n log n) time.
    std::vector<AffiliationID> get_affiliations_distance_increasing();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Iterates over all elements in a hash map to find an affiliation with a specific coordinate.
    AffiliationID find_affiliation_with_coord(Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses and modifies an element in a hash map, which is a constant time operation.
    bool change_affiliation_coord(AffiliationID id, Coord newcoord);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Inserts an element into a hash map and updates affiliations, taking O(k) time on average, where k is the number of affiliations.
    bool add_publication(PublicationID id, Name const& name, Year year, const std::vector<AffiliationID> & affiliations);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses the size of a hash map, which is a constant time operation.
    std::vector<PublicationID> all_publications();

    // Estimate of performance: O(1)
    // Short rationale for estimate: Accesses an element in a hash map, which is a constant time operation.
    Name get_publication_name(PublicationID id);

    // Estimate of performance: O(1)
    // Short rationale for estimate: Accesses an element in a hash map, which is a constant time operation.
    Year get_publication_year(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses an element in a hash map, which is a constant time operation.
    std::vector<AffiliationID> get_affiliations(PublicationID id);

    // Estimate of performance: O(n) on average
    // Short rationale for estimate: Accesses and modifies elements in hash maps, which is an average constant time operation.
    bool add_reference(PublicationID id, PublicationID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses an element in a hash map, which is a constant time operation.
    std::vector<PublicationID> get_direct_references(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses and modifies elements in hash maps, which is an average constant time operation.
    bool add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Accesses an element in a hash map, which is an average constant time operation.
    std::vector<PublicationID> get_publications(AffiliationID id);

    // Estimate of performance: O(1)
    // Short rationale for estimate: Accesses an element in a hash map, which is a constant time operation.
    PublicationID get_parent(PublicationID id);

    // Estimate of performance: O(n * log(n))
    // Short rationale for estimate: Iterates over elements and sorts them based on year, taking O(m log m) time, where m is the number of publications.
    std::vector<std::pair<Year, PublicationID>> get_publications_after(AffiliationID affiliationid, Year year);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Performs depth-first search over a graph structure, where n is the number of nodes and e is the number of edges.
    std::vector<PublicationID> get_referenced_by_chain(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Performs depth-first search over a graph structure, where n is the number of nodes and e is the number of edges.
    std::vector<PublicationID> get_all_references(PublicationID id);

    // Not implemented yet
    std::vector<AffiliationID> get_affiliations_closest_to(Coord xy);

    // Not implemented yet
    bool remove_affiliation(AffiliationID id);

    // Not implemented yet
    PublicationID get_closest_common_parent(PublicationID id1, PublicationID id2);

    // Not implemented yet
    bool remove_publication(PublicationID publicationid);

private:
    std::unordered_map<PublicationID, PublicationInfo> publications;
    std::unordered_map<AffiliationID, std::vector<PublicationID>> affiliations_publications;
    std::unordered_map<AffiliationID, std::tuple<Name, Coord>> affiliations;
    std::unordered_map<PublicationID, std::vector<PublicationID>> reverse_references;
    std::unordered_map<Coord, std::vector<AffiliationID>, CoordHash> coord_to_affiliation;
    // Additional members for optimization
    std::set<std::pair<Name, AffiliationID>> sorted_affiliations_by_name;
    std::set<std::pair<Distance, AffiliationID>> sorted_affiliations_by_distance;
    bool affiliations_sorted_by_name = true;
    bool affiliations_sorted_by_distance = true;

    // Utility functions
    void update_sorted_affiliations_by_name();
    void update_sorted_affiliations_by_distance();
    Distance calculate_distance_from_origin(Coord coord);
    // Utility function to get ancestors of a publication

    std::vector<PublicationID> get_ancestors(PublicationID id);
};

#endif // DATASTRUCTURES_HH
