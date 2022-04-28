#include <Rcpp.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//


struct Segment {
    long start, end, width;
    std::string id;
    Segment(long start_, long end_, std::string id_)
        : start(start_),
          end(end_),
          width(end_ - start_ + 1),
          id(id_) {}
};

std::ostream &operator<<(std::ostream &out, const Segment &data) {
    out << data.start << "->" << data.end << " = " << data.width;
    return out;
}


// [[Rcpp::export]]
CharacterVector assign_to_layer(const DataFrame& df) {
    // https://www.techiedelight.com/weighted-interval-scheduling-problem-using-lis/
    IntegerVector starts = df["START"];
    IntegerVector ends = df["END"];
    CharacterVector ids = df["ID"];
    assert(starts.length() == ends.length());

    std::vector<Segment> segments;
    for (int i = 0; i < starts.length(); ++i) {
        segments.emplace_back(starts[i], ends[i], Rcpp::as<std::string>(ids[i]));
    }
    
    std::sort(segments.begin(), segments.end(),
              [](const Segment& a, const Segment& b) {
                  return a.start < b.start;
              });
    
    int n = segments.size();
    
    if (n == 0) {
        return CharacterVector{};
    }
    
    std::vector<IntegerVector> segment_indices(n);
    std::vector<int> max_scores(n, 0);
    
    for (int i = 0; i < n; i++)
    {
        // consider each `j` less than `i`
        for (int j = 0; j < i; j++)
        {
            // update i'th job if the j'th job is non-conflicting and leading to the
            // maximum profit
            if (segments[j].end <= segments[i].start && max_scores[i] < max_scores[j])
            {
                segment_indices[i] = segment_indices[j];
                max_scores[i] = max_scores[j];
            }
        }
        
        // end current task with i'th job
        segment_indices[i].push_back(i);
        max_scores[i] += segments[i].width;
    }
    auto max_index = std::distance(max_scores.begin(), std::max_element(max_scores.begin(), max_scores.end()));
    
    CharacterVector output_ids;
    for (int index : segment_indices[max_index]) {
        output_ids.push_back(segments[index].id);
    }
    return output_ids;
}
