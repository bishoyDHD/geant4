# Geant4InterfaceOptions - module to configure build options for Geant4 UI/Vis
#
# Rather than separate the UI and visualization options, we simply ask if
# the user wants to build support for a particular tool, e.g. Qt.
#
# Specific UI/Vis options are also handled here.
#

if(UNIX)
    #--------------------------------------------------------------------------
    # XAW (X11 Athena) Support (UI only)
    #option(GEANT4_USE_XAW "Build Geant4 with XAW (X11 Athena) support" OFF)

    if(GEANT4_USE_XAW)
        # Find and configure XAW...
    endif()

    #--------------------------------------------------------------------------
    # Xm (Motif) Support (UI and Vis)
    #option(GEANT4_USE_XM "Build Geant4 with Xm (X11-Motif) support" OFF)

    if(GEANT4_USE_XM)
        # Find and configure Xm...
    endif()

    #--------------------------------------------------------------------------
    # X11 Support for RayTracer driver
    #option(GEANT4_USE_RAYTRACERX "Build RayTracer driver with X11 support" OFF)

    if(GEANT4_USE_RAYTRACERX)
        # Find and configure X11...
    endif()

    #-------------------------------------------------------------------------
    # Support for Client/Server DAWN driver
    # mark as advanced because user should know what they're doing to use this
    #option(GEANT4_USE_NETWORKDAWN "Build Dawn driver with Client/Server
    #support" OFF)
    #
    # Possible headers checks for needed network parts?
    #
    #mark_as_advanced(GEANT4_USE_NETWORKDAWN)

    #--------------------------------------------------------------------------
    # Support for Client/Server VRML driver
    # mark as advanced because user should know what they're doing to use this
    #option(GEANT4_USE_NETWORKVRML "Build VRML driver with Client/Server
    #support" OFF)
    #
    # Possible header checks for needed network parts?
    #
    #mark_as_advanced(GEANT4_USE_NETWORKVRML)
endif()

#------------------------------------------------------------------------------
# Qt Support (UI and Vis)
#option(GEANT4_USE_QT "Build Geant4 with Qt support" OFF)

if(GEANT4_USE_QT)
    # Find and configure Qt - we prefer Qt4, but fall back to Qt3...
    # This may need improvement... and need to work out how to use
    # flags in builds - i.e. don't compile Qt stuff if Qt not found...
    find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL)

    if(NOT QT_FOUND)
        message(STATUS "failed to find Qt4, trying Qt3...")
        find_package(Qt3 REQUIRED)
        if(NOT QT_FOUND)
            message(SEND_ERROR "Qt3 could not be found.")
        endif()
    endif()
endif()
