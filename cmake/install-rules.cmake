install(
    TARGETS athena_exe
    RUNTIME COMPONENT athena_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
