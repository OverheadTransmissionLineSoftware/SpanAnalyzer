# global settngs
cmake_minimum_required (VERSION 3.5.1)

project (SpanAnalyzer)
set (CMAKE_CXX_STANDARD 11)
set (APPCOMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/AppCommon)
set (SPANANALYZER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set (MODELS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Models)
set (WXWIDGETS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../external/wxWidgets)

# defines include directories
include_directories (${SPANANALYZER_SOURCE_DIR}/include)
include_directories (${SPANANALYZER_SOURCE_DIR}/res)
include_directories (${APPCOMMON_SOURCE_DIR}/include)
include_directories (${APPCOMMON_SOURCE_DIR}/res)
include_directories (${MODELS_SOURCE_DIR}/include)
include_directories (${WXWIDGETS_SOURCE_DIR}/include)

# defines common source files
set (APPCOMMON_SRC_FILES
  ${APPCOMMON_SOURCE_DIR}/src/editors/cable_editor_dialog.cc
  ${APPCOMMON_SOURCE_DIR}/src/editors/weather_load_case_editor_dialog.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/circle_data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/circle_renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/line_data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/line_renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/plot_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/plot_axis.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/plot_pane_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/point_data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/point_renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/text_data_set_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/graphics/text_renderer_2d.cc
  ${APPCOMMON_SOURCE_DIR}/src/units/cable_constraint_unit_converter.cc
  ${APPCOMMON_SOURCE_DIR}/src/units/cable_unit_converter.cc
  ${APPCOMMON_SOURCE_DIR}/src/units/line_cable_unit_converter.cc
  ${APPCOMMON_SOURCE_DIR}/src/units/weather_load_case_unit_converter.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/error_message_dialog.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/log_pane.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/report_table.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/status_bar_log.cc
  ${APPCOMMON_SOURCE_DIR}/src/widgets/timer.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/cable_constraint_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/cable_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/color_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/line_cable_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/vector_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/weather_load_case_xml_handler.cc
  ${APPCOMMON_SOURCE_DIR}/src/xml/xml_handler.cc
)

# defines SpanAnalyzer source files
set (SPANANALYZER_SRC_FILES
  ${SPANANALYZER_SOURCE_DIR}/res/resources.cc
  ${SPANANALYZER_SOURCE_DIR}/src/analysis_controller.cc
  ${SPANANALYZER_SOURCE_DIR}/src/analysis_filter_editor_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/analysis_filter_manager_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/analysis_filter_xml_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/cable_constraint_editor_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/cable_constraint_manager_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/cable_file_manager_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/cable_file_xml_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/cable_plot_options_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/cable_plot_pane.cc
  ${SPANANALYZER_SOURCE_DIR}/src/edit_pane.cc
  ${SPANANALYZER_SOURCE_DIR}/src/file_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/plan_plot_options_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/plan_plot_pane.cc
  ${SPANANALYZER_SOURCE_DIR}/src/preferences_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/profile_plot_options_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/profile_plot_pane.cc
  ${SPANANALYZER_SOURCE_DIR}/src/results_pane.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_app.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_config_xml_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_data_xml_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_doc.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_doc_commands.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_doc_xml_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_frame.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_printout.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_analyzer_view.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_editor_dialog.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_unit_converter.cc
  ${SPANANALYZER_SOURCE_DIR}/src/span_xml_handler.cc
  ${SPANANALYZER_SOURCE_DIR}/src/weather_load_case_manager_dialog.cc
)

# defines SpanAnalyzer + AppCommon resource files
set (SPANANALYZER_RESOURCE_FILES
  ${SPANANALYZER_SOURCE_DIR}/external/AppCommon/res/xrc/cable_editor_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/external/AppCommon/res/xrc/error_message_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/external/AppCommon/res/xrc/weather_load_case_editor_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/analysis_filter_editor_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/analysis_filter_manager_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/cable_constraint_editor_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/cable_constraint_manager_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/cable_file_manager_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/cable_plot_options_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/edit_pane.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/plan_plot_options_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/preferences_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/profile_plot_options_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/results_pane.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/span_analyzer_menubar.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/span_editor_dialog.xrc
  ${SPANANALYZER_SOURCE_DIR}/res/xrc/weather_load_case_manager_dialog.xrc
)

# adds Models libraries
add_subdirectory (${MODELS_SOURCE_DIR}/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/Models)

# adds wxWidgets libraries
# using wx-config in git subdirectory
# - find_package() command won't locate wxWidgets as a subdirectory on unix
# - add_subdirectory() command has a build error
# - ExternalProject looks complicated

# gets wx-config directory
if (CMAKE_BUILD_TYPE MATCHES Debug)
  set (WXCONFIG_DIR ${WXWIDGETS_SOURCE_DIR}/build-debug)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
  set (WXCONFIG_DIR ${WXWIDGETS_SOURCE_DIR}/build-release)
endif ()

# gets wxWidgets compiler flags
execute_process (COMMAND ${WXCONFIG_DIR}/wx-config --cxxflags
                 OUTPUT_VARIABLE WXWIDGETS_COMPILER_FLAGS)
separate_arguments(WXWIDGETS_COMPILER_FLAGS UNIX_COMMAND "${WXWIDGETS_COMPILER_FLAGS}")

# gets wxWidgets linker flags
execute_process (COMMAND ${WXCONFIG_DIR}/wx-config --libs all
                 OUTPUT_VARIABLE WXWIDGETS_LINKER_FLAGS)
separate_arguments(WXWIDGETS_LINKER_FLAGS UNIX_COMMAND "${WXWIDGETS_LINKER_FLAGS}")

# compiles resource files
execute_process (COMMAND ${WXCONFIG_DIR}/utils/wxrc/wxrc -v -c
                 -o ${SPANANALYZER_SOURCE_DIR}/res/resources.cc
                 ${SPANANALYZER_RESOURCE_FILES})

# defines executable
add_executable (SpanAnalyzer ${APPCOMMON_SRC_FILES} ${SPANANALYZER_SRC_FILES})
target_compile_options (SpanAnalyzer PUBLIC ${WXWIDGETS_COMPILER_FLAGS})
target_link_libraries (SpanAnalyzer LINK_PUBLIC
  otlsmodels_sagtension
  otlsmodels_transmissionline
  otlsmodels_base
  ${WXWIDGETS_LINKER_FLAGS})

# prints out all variables for debugging
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
