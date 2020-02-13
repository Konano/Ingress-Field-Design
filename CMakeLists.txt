cmake_minimum_required(VERSION 3.10)

project(Ingress-Field-Design)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/release.${CMAKE_SYSTEM_NAME}.${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

add_subdirectory(lib/json)

add_executable(BAF src/BAF/baf-choose.cpp)
target_link_libraries(BAF jsoncpp_lib)
set_target_properties(BAF PROPERTIES OUTPUT_NAME baf-choose)
install(TARGETS BAF
        DESTINATION BAF)

add_executable(HCF src/HCF/choose.cpp)
target_link_libraries(HCF jsoncpp_lib)
set_target_properties(HCF PROPERTIES OUTPUT_NAME choose)
install(TARGETS HCF
        DESTINATION HCF)
install(FILES src/HCF/result-sample.txt src/HCF/portal-sample.txt
        DESTINATION HCF)

add_executable(HCF-MoreKeys src/HCF-MoreKeys/morekey.cpp)
target_link_libraries(HCF-MoreKeys jsoncpp_lib)
set_target_properties(HCF-MoreKeys PROPERTIES OUTPUT_NAME morekey)
install(TARGETS HCF-MoreKeys
        DESTINATION HCF-MoreKeys)
install(FILES src/HCF-MoreKeys/result-sample.txt src/HCF-MoreKeys/portal-sample.txt
        DESTINATION HCF-MoreKeys)

add_executable(HCF-RESWUEV1 src/HCF-RESWUEV1/reswue.cpp)
target_link_libraries(HCF-RESWUEV1 jsoncpp_lib)
set_target_properties(HCF-RESWUEV1 PROPERTIES OUTPUT_NAME reswue)
install(TARGETS HCF-RESWUEV1
        DESTINATION HCF-RESWUEV1)

add_executable(HCF-SingleWork src/HCF-SingleWork/act.cpp)
target_link_libraries(HCF-SingleWork jsoncpp_lib)
set_target_properties(HCF-SingleWork PROPERTIES OUTPUT_NAME act)
install(TARGETS HCF-SingleWork
        DESTINATION HCF-SingleWork)
install(FILES src/HCF-SingleWork/act-sample.txt src/HCF-SingleWork/way-sample.txt
        DESTINATION HCF-SingleWork)

add_executable(MultField src/MultField/mult-choose.cpp)
target_link_libraries(MultField jsoncpp_lib)
set_target_properties(MultField PROPERTIES OUTPUT_NAME mult-choose)
install(TARGETS MultField
        DESTINATION MultField)
install(FILES src/MultField/mult-result-sample.txt src/MultField/mult-portal-sample.txt
        DESTINATION MultField)

add_executable(TriField src/TriField/tri-choose.cpp)
target_link_libraries(TriField jsoncpp_lib)
set_target_properties(TriField PROPERTIES OUTPUT_NAME tri-choose)
install(TARGETS TriField
        DESTINATION TriField)
install(FILES src/TriField/result-sample.txt
        DESTINATION TriField)

install(FILES src/portals2json.user.js src/LABEL
        DESTINATION .)