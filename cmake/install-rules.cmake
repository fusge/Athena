install(
    TARGETS athena_exe
    RUNTIME COMPONENT Athena_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
