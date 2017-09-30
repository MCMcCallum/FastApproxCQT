#
# Created by: Matthew McCallum
# 4th June 2017
#
# Pulls in dependencies for the fast CQT project
#
# Dependencies covered here:
#   - GTest
#   - IPP
#   - GYP
#

echo " "
echo "Installing Dependencies..."
echo " "

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd "${ROOT_DIR}"

    # Prepare thirdparty paths

    THIRDPARTY_DIR="${ROOT_DIR}"/thirdparty/
    LIB_DIR="${THIRDPARTY_DIR}/lib"
    INCLUDE_DIR="${THIRDPARTY_DIR}/include"
    mkdir -p "${LIB_DIR}"
    mkdir -p "${INCLUDE_DIR}"

    ###################################################
    #                     GTEST                       #
    ###################################################

    echo " "
    echo "Installing GTest..."
    echo " "

    GTEST_BUILD_DIR="${THIRDPARTY_DIR}/gtest/"
    mkdir -p "${GTEST_BUILD_DIR}"
    pushd "${GTEST_BUILD_DIR}"

        GTEST_LIB_DIR="./googletest"

        if [ ! -e "${GTEST_LIB_DIR}/README.md" ]; then
            git clone --branch release-1.8.0 https://github.com/google/googletest.git ${GTEST_LIB_DIR} # Should only do this if it is not already downloaded...
        fi

        if [ ! -e ${LIB_DIR}/libgtest.a ]; then
            pushd "${GTEST_LIB_DIR}/googletest"
                cmake .
                make gtest
                make gtest_main
            popd

            cp "${GTEST_LIB_DIR}"/googletest/libgtest.a "${LIB_DIR}"/libgtest.a
            cp "${GTEST_LIB_DIR}"/googletest/libgtest_main.a "${LIB_DIR}"/libgtest_main.a
            cp -r "${GTEST_LIB_DIR}"/googletest/include/ "${INCLUDE_DIR}"
        fi

    popd

    ###################################################
    #                      IPP                        #
    ###################################################

    echo " "
    echo "Installing IPP..."
    echo " "

    IPP_BASE_DIR="/opt/intel/ipp/"

    if [ ! -e "/opt/intel/ipp/include/ipp.h" ]; then
        echo "ERROR: Could not find IPP library."
        echo "This must be manually downloaded and installed."
        echo "Go here for more info: https://software.intel.com/en-us/articles/free-ipp"
        echo " "
    fi

    if [ ! -e "${LIB_DIR}/libippsmerged.a" ]; then
        cp "${IPP_BASE_DIR}/lib/libippcore.a" "${LIB_DIR}/libippcore.a"
        cp "${IPP_BASE_DIR}/lib/libipps.a" "${LIB_DIR}/libipps.a"
        cp "${IPP_BASE_DIR}/lib/libippvm.a" "${LIB_DIR}/libippsvm.a"

        mkdir "${INCLUDE_DIR}/ipp/"
        cp "${IPP_BASE_DIR}/include/"*.h "${INCLUDE_DIR}/ipp/"
    fi

    echo " "
    echo "Installing GYP..."
    echo " "

    if [[ $( which gyp ) == "" ]]; then

        GYP_BUILD_DIR="${THIRDPARTY_DIR}/gyp/"
        mkdir -p "${GYP_BUILD_DIR}"
        pushd "${GYP_BUILD_DIR}"

            git clone https://chromium.googlesource.com/external/gyp.git
            GYP_LIB_DIR="./gyp"
            pushd ${GYP_LIB_DIR}
                echo " "
                echo "Going to need your computer password to install GYP..."
                echo " "
                sudo python setup.py install 
            popd

        popd

    fi
    
popd



