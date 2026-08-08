############################################################################
# src/app/CMakeLists.txt
############################################################################

if(CONFIG_ARDUPROF_APP)

    file(GLOB SRCS arduprof_main.cxx thread/*.cxx peripheral/*.cxx)

    nuttx_add_application(
        NAME
        ${CONFIG_ARDUPROF_APP_PROGNAME}
        SRCS ${SRCS}
        STACKSIZE
        ${CONFIG_ARDUPROF_APP_STACKSIZE}
        PRIORITY
        ${CONFIG_ARDUPROF_APP_PRIORITY})
endif()
