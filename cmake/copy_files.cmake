macro(copy_files GLOBPAT DESTINATION)
  file(GLOB COPY_FILES
    ${GLOBPAT})
  foreach(FILENAME ${COPY_FILES})
    set(SRC "${FILENAME}")
    set(DST "${DESTINATION}")

    add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
      )
  endforeach(FILENAME)
endmacro(copy_files)