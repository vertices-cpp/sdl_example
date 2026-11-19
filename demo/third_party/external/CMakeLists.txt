cmake_minimum_required(VERSION 3.17)
project(external)

# ============================================================
# 1. 基础库
# ============================================================
add_subdirectory(zlib)
add_subdirectory(tinyxml2)
add_subdirectory(minizip)
add_subdirectory(edtaa3func)
add_subdirectory(md5)

# ---- FreeType ----
set(FT_DISABLE_ZLIB     TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HVF      TRUE CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF  CACHE BOOL "" FORCE)
add_subdirectory(freetype ${CMAKE_BINARY_DIR}/freetype)

# ---- Chipmunk ----
set(BUILD_DEMOS    OFF CACHE BOOL "" FORCE)
set(INSTALL_DEMOS  OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(INSTALL_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(Chipmunk2D ${CMAKE_BINARY_DIR}/Chipmunk2D)

# ============================================================
# 2. OpenSSL（当子目录编，需要你已改过 openssl-cmake）
# ============================================================
add_subdirectory(openssl-OpenSSL_1_1_1s
                 ${CMAKE_BINARY_DIR}/openssl)

# ============================================================
# 3. libwebsockets
# ============================================================
set(LWS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libwebsockets-4.0.1")

set(LWS_WITH_SSL              OFF CACHE BOOL "" FORCE)
set(LWS_WITH_MBEDTLS          OFF CACHE BOOL "" FORCE)
set(LWS_WITH_WOLFSSL          OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(LWS_WITH_STATIC  OFF CACHE BOOL "" FORCE)
    set(LWS_WITH_SHARED  ON  CACHE BOOL "" FORCE)
else()
    set(LWS_WITH_STATIC  ON  CACHE BOOL "" FORCE)
    set(LWS_WITH_SHARED  OFF CACHE BOOL "" FORCE)
endif()

set(LWS_WITHOUT_TESTAPPS      ON  CACHE BOOL "" FORCE)
set(LWS_WITHOUT_TEST_SERVER   ON  CACHE BOOL "" FORCE)
set(LWS_WITHOUT_TEST_CLIENT   ON  CACHE BOOL "" FORCE)
set(LWS_WITHOUT_TEST_PING     ON  CACHE BOOL "" FORCE)
set(LWS_WITHOUT_EXTENSIONS    ON  CACHE BOOL "" FORCE)
set(LWS_WITH_LIBEV            OFF CACHE BOOL "" FORCE)
set(LWS_WITH_LIBUV            OFF CACHE BOOL "" FORCE)
set(LWS_WITH_LIBEVENT         OFF CACHE BOOL "" FORCE)
set(LWS_WITH_GLIB             OFF CACHE BOOL "" FORCE)
set(LWS_WITH_HTTP2            OFF CACHE BOOL "" FORCE)
set(LWS_IPV6                  OFF CACHE BOOL "" FORCE)
set(LWS_WITH_ZLIB             OFF CACHE BOOL "" FORCE)
set(LWS_WITH_MINIZ            OFF CACHE BOOL "" FORCE)
set(LWS_WITHOUT_DAEMONIZE     ON  CACHE BOOL "" FORCE)
set(DISABLE_WERROR            ON  CACHE BOOL "" FORCE)
set(LWS_WITH_EXPORT_LWSTARGETS OFF CACHE BOOL "" FORCE)

add_subdirectory(${LWS_SRC_DIR} ${CMAKE_BINARY_DIR}/libwebsockets)


#需要头
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/openssl-OpenSSL_1_1_1s/include"
    "${CMAKE_BINARY_DIR}/openssl/include"
)

# ============================================================
# 4. curl
# ============================================================
if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Android")
    if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
        set(_CURL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/curl/lib/android/arm64-v8a")
    elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
        set(_CURL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/curl/lib/android/armeabi-v7a")
    else()
        message(FATAL_ERROR "Unknown Android ABI: ${CMAKE_ANDROID_ARCH_ABI}")
    endif()

    add_library(libcurl_static STATIC IMPORTED)
    set_target_properties(libcurl_static PROPERTIES
        IMPORTED_LOCATION "${_CURL_LIB_DIR}/libcurl.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/curl/include"
    )
else()
    
 
set(BUILD_EXAMPLES        OFF CACHE BOOL "" FORCE)   # 不编示例
set(BUILD_LIBCURL_DOCS    OFF CACHE BOOL "" FORCE)   # 不编文档
set(BUILD_MISC_DOCS       OFF CACHE BOOL "" FORCE)   # 不编杂项文档
set(ENABLE_CURL_MANUAL    OFF CACHE BOOL "" FORCE)   # 不编手册
set(CURL_INSTALL_CMAKE_DIR "" CACHE STRING "" FORCE) # 不生成 install 规则
set(CURL_ENABLE_EXPORT_TARGET OFF CACHE BOOL "" FORCE) # 不导出
    set(CURL_USE_OPENSSL   ON CACHE BOOL "" FORCE)
    set(CURL_USE_MBEDTLS   OFF CACHE BOOL "" FORCE)
    set(CURL_USE_GNUTLS    OFF CACHE BOOL "" FORCE)
    set(CURL_USE_WOLFSSL   OFF CACHE BOOL "" FORCE)
    set(CURL_USE_BEARSSL   OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBRESSL  OFF CACHE BOOL "" FORCE)
    set(CURL_USE_SCHANNEL  OFF CACHE BOOL "" FORCE)
    set(CURL_USE_RUSTLS    OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL    OFF CACHE BOOL "" FORCE)

    set(HTTP_ONLY              ON  CACHE BOOL "" FORCE)
    set(BUILD_CURL_EXE         OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS      OFF CACHE BOOL "" FORCE)
    set(BUILD_STATIC_LIBS      ON  CACHE BOOL "" FORCE)
    set(BUILD_TESTING          OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TESTS     ON  CACHE BOOL "" FORCE)

    set(CURL_DISABLE_LDAP      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_FTP       ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_FILE      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TELNET    ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_DICT      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TFTP      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_RTSP      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_POP3      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_IMAP      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_SMTP      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_SMB       ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_GOPHER    ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_MQTT      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_PROXY     ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_DOH       ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_ALTSVC    ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_HSTS      ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_AWS       ON  CACHE BOOL "" FORCE)
    set(CURL_DISABLE_WEBSOCKETS ON CACHE BOOL "" FORCE)
    add_subdirectory(curl-8.15.0 ${CMAKE_BINARY_DIR}/curl)
endif()

# ============================================================
# 5. 统一入口
# ============================================================
add_library(external INTERFACE)

if(WIN32)
    target_link_directories(external INTERFACE
        "${CMAKE_BINARY_DIR}/libwebsockets/lib/$<CONFIG>")
endif()

if(WIN32)
    target_include_directories(external INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/Chipmunk2D/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/curl-8.15.0/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/md5/"
        "${CMAKE_BINARY_DIR}/curl/include"
        "${LWS_SRC_DIR}/include"
        "${CMAKE_BINARY_DIR}/libwebsockets/include"
    )

    target_link_libraries(external INTERFACE
		ext_md5
        edtaa3func
        zlib
        tinyxml2
        minizip
        freetype
        chipmunk_static
        websockets
        libcurl_static
    )
else()
    target_include_directories(external INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/Chipmunk2D/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/curl/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/md5/"
        "${LWS_SRC_DIR}/include"
        "${CMAKE_BINARY_DIR}/libwebsockets/include"
    )

    target_link_libraries(external INTERFACE
		ext_md5
        edtaa3func
        zlib
        tinyxml2
        minizip
        freetype
        chipmunk_static
        websockets_static
        libcurl_static
        ssl
        crypto
        log
        m
    )
endif()

# ============================================================
# 6. VS 分组
# ============================================================
if(WIN32)#VC生成的项目名带_shared
	foreach(tgt 
	ext_md5
	edtaa3func
	zlib
	ssl openssl
    crypto  
   tinyxml2 freetype chipmunk_static box2d websockets_shared libcurl_static)
		if(TARGET ${tgt})
			get_target_property(_aliased ${tgt} ALIASED_TARGET)
		 
			if(NOT _aliased)
				set_target_properties(${tgt} PROPERTIES FOLDER "External")
			endif()
		endif()
	endforeach()
else()
	foreach(tgt 
	ext_md5
	edtaa3func
	zlib
	openssl
	ssl
    crypto
	tinyxml2 freetype chipmunk_static box2d websockets_static libcurl_static)
		if(TARGET ${tgt})
			get_target_property(_aliased ${tgt} ALIASED_TARGET)
		 
			if(NOT _aliased)
				set_target_properties(${tgt} PROPERTIES FOLDER "External")
			endif()
		endif()
	endforeach()
endif()

# curl 内部 target
foreach(tgt curl_uninstall dist GENHDR libcurl_object)
    if(TARGET ${tgt})
        set_target_properties(${tgt} PROPERTIES FOLDER "External/curl")
    endif()
endforeach()

# CTest 仪表盘 target
foreach(tgt Continuous Experimental Nightly NightlyMemoryCheck)
    if(TARGET ${tgt})
        set_target_properties(${tgt} PROPERTIES FOLDER "Utils/CTest")
    endif()
endforeach()
 