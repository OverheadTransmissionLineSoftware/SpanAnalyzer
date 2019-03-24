# data precision

## fixed vs floating
SpanAnalyzer uses both fixed and floating precision data. Each piece of data
will use a specific type of precision based on its usage.

Fixed (decimal) precision is used most often when the precision needs to match
other similar data. This is important in situations such as coordinate systems.

Floating (entire number) precision is used most often when the data unit styles
differ in magnitude. This allows xml handlers to capture the data similarly in
both unit styles so no precision is lost to rounding.

## digits
SpanAnalyzer assigns digits of precision (i.e. significant figures) to each
piece of data. For the same piece of data, the precision will vary based on
where it is used.

The data displayed in the graphical user interface has the lowest precision.
This is done so the user is looking at meaningful data that doesn't exceed the
precision that the analysis/design actually needs. The minimum digits of
precision can be increased as needed, but the minimums are:
* length = 2
* force = 1
* stress = 1
* temperature = 1

The data stored in app memory and xml files typically has the highest precision.
This is done so the app can apply conversions back and forth between unit
systems and/or styles without introducing errors due to rounding. For fixed
precision data, the digits of precision is one more than the graphical user
interface. For floating precision, the default is set to an arbitrary 6 digits.
