// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_SPANANALYZERDATA_H_
#define OTLS_SPANANALYZER_SPANANALYZERDATA_H_

#include <vector>

#include "models/transmissionline/cable.h"

/// \par OVERVIEW
///
/// This struct stores the application data. This information is referenced by a
/// SpanAnalyzerDocument.
struct SpanAnalyzerData {
 public:
  /// \var cables
  ///   The cables.
  std::vector<Cable> cables;
};

#endif //  OTLS_SPANANALYZER_SPANANALYZERDATA_H_
