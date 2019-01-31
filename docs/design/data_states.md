# data states

Data that is being used in SpanAnalyzer can be in several states. Data is
transitioned between the states using XmlHandlers and UnitConverters. There is
a corresponding XmlHandler and UnitConverter for every data type that
needs to be transitioned.
```
             command processor
                    ˄
                    |
                    |
                    |
                    |
                    |
                    ˅
analysis <-----> app data <-----> gui
                    ˄
                    |
                    |
                    |
                    |
                    |
                    ˅
                   disk
```

## app data
This is the default state where active data is stored in memory.
* format: `objects`
* unit style: `consistent`

## gui
This is the state where data is being displayed to the user.
* format: `window objects`
* unit style: `different`

## disk
This is the state where data is stored onto the disk. SpanAnalyzer does not
have to be running for data in this state.
* format: `xml`
* unit style: `different`

## command processor
This is the state where inactive data is stored in memory. It allows undo/redo
for edit commands.
* format: `xml`
* unit style: `consistent`

## analysis
This is the state where data is being analyzed using the Models library.
* format: `objects`
* unit style: `consistent`
