cmake_minimum_required(VERSION 3.16)
project(Zc-Manager-client LANGUAGES CXX)

#Qxlsx 操作Excel表格的库！
#MSVC      
#MinGW
#cmake -B build ./QXlsx/ -DCMAKE_BUILD_TYPE=Release
#cmake --build build --target INSTALL       #Debug
#cmake --build build --config Release --target INSTALL #Release
#include(qt.cmake)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("./thrid-party/QXlsx")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Network
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
    ContextHolder.h
    ContextHolder.cpp

    common/base/GlobalSignalHub.hpp

    common/notify/NotifyTipBox.h
    common/notify/NotifyTipBox.cpp
    common/notify/NotifyTipManager.h
    common/notify/NotifyTipManager.cpp
    common/widgets/ClickLabel.h
    common/widgets/ClickLabel.cpp
    common/widgets/HoverButton.h
    common/widgets/HoverButton.cpp
    common/widgets/PermButton.h
    common/widgets/PermButton.cpp
    common/widgets/ComboBox.h
    common/widgets/ComboBox.cpp
    common/widgets/MessageBox.h
    common/widgets/MessageBox.cpp
    common/widgets/MessageBox.ui
    common/widgets/AvatarChoose.h
    common/widgets/AvatarChoose.cpp
    common/widgets/AvatarChoose.ui
    common/widgets/AvatarCropperBox.h
    common/widgets/AvatarCropperBox.cpp
    common/widgets/NavTreeView.h
    common/widgets/NavTreeView.cpp
    common/widgets/DropDown.h
    common/widgets/DropDown.cpp
    common/widgets/IconSelectView.h
    common/widgets/IconSelectView.cpp
    common/widgets/TreeComboBox.h
    common/widgets/TreeComboBox.cpp
    common/widgets/CheckBoxHeaderView.h
    common/widgets/CheckBoxHeaderView.cpp
    common/widgets/PaginationToolBar.h
    common/widgets/PaginationToolBar.cpp 
    common/widgets/FlowLayout.h
    common/widgets/FlowLayout.cpp 
    common/widgets/TagPicker.h
    common/widgets/TagPicker.cpp 
    common/widgets/XlsxUploadDlg.h
    common/widgets/XlsxUploadDlg.cpp 
    common/widgets/XlsxUploadDlg.ui
    common/widgets/datetime/CalendarWidget.ui
    common/widgets/datetime/CalendarWidget.h
    common/widgets/datetime/CalendarWidget.cpp
    common/widgets/datetime/DualCalendarWidget.ui
    common/widgets/datetime/DualCalendarWidget.h
    common/widgets/datetime/DualCalendarWidget.cpp
    common/widgets/datetime/DateEdit.h
    common/widgets/datetime/DateEdit.cpp
    common/widgets/datetime/DateRangePicker.h
    common/widgets/datetime/DateRangePicker.cpp

    common/utils/CaptchaMaker.h
    common/utils/CaptchaMaker.cpp
    common/utils/OverlayWidget.h
    common/utils/OverlayWidget.cpp
    common/utils/Utils.h
    common/utils/Utils.cpp
    common/utils/StringUtils.h
    common/utils/StringUtils.cpp
    common/utils/ExcelUtil.h
    common/utils/ExcelUtil.cpp

    common/config/Config.h
    common/config/Config.cpp

    common/delegate/NavTreeDelegate.h
    common/delegate/NavTreeDelegate.cpp
    common/delegate/MappingDelegate.h
    common/delegate/MappingDelegate.cpp
    common/delegate/ButtonDelegate.h
    common/delegate/ButtonDelegate.cpp
    common/delegate/SwitchDelegate.h
    common/delegate/SwitchDelegate.cpp
    common/delegate/CheckBoxDelegate.h
    common/delegate/CheckBoxDelegate.cpp

    common/network/HttpMgr.h
    common/network/HttpMgr.cpp

    modules/login/LoginPage.h
    modules/login/LoginPage.cpp
    modules/login/LoginPage.ui
    modules/home/HomePage.h
    modules/home/HomePage.cpp
    modules/home/HomePage.ui
    modules/home/Home.h
    modules/home/Home.cpp
    modules/home/Home.ui
    modules/personal-info/PersonalInfoPage.h
    modules/personal-info/PersonalInfoPage.cpp
    modules/personal-info/PersonalInfoPage.ui
    modules/personal-info/PersonalInfo.h
    modules/personal-info/PersonalInfo.cpp
    modules/personal-info/PersonalInfo.ui
    modules/personal-info/BaseProfile.h
    modules/personal-info/BaseProfile.cpp
    modules/personal-info/BaseProfile.ui
    modules/menu/MenuMgrPage.h
    modules/menu/MenuMgrPage.cpp
    modules/menu/MenuMgrPage.ui
    modules/menu/MenuAddDlg.h
    modules/menu/MenuAddDlg.cpp
    modules/menu/MenuAddDlg.ui
    modules/menu/MenuEditDlg.h
    modules/menu/MenuEditDlg.cpp
    modules/menu/MenuEditDlg.ui

    modules/user/UserMgrPage.h
    modules/user/UserMgrPage.cpp
    modules/user/UserMgrPage.ui
    modules/user/UserAddDlg.h
    modules/user/UserAddDlg.cpp
    modules/user/UserAddDlg.ui
    modules/user/UserEditDlg.h
    modules/user/UserEditDlg.cpp
    modules/user/UserEditDlg.ui

    modules/role/RoleMgrPage.h
    modules/role/RoleMgrPage.cpp
    modules/role/RoleMgrPage.ui
    modules/role/RoleAddDlg.h
    modules/role/RoleAddDlg.cpp
    modules/role/RoleAddDlg.ui
    modules/role/RoleEditDlg.h
    modules/role/RoleEditDlg.cpp
    modules/role/RoleEditDlg.ui
    modules/role/AssignUserRolePage.h
    modules/role/AssignUserRolePage.cpp
    modules/role/AssignUserRolePage.ui
    modules/role/AssignUserRoleAddDlg.h
    modules/role/AssignUserRoleAddDlg.cpp
    modules/role/AssignUserRoleAddDlg.ui

    modules/dept/DeptMgrPage.h
    modules/dept/DeptMgrPage.cpp
    modules/dept/DeptMgrPage.ui
    modules/dept/DeptAddDlg.h
    modules/dept/DeptAddDlg.cpp
    modules/dept/DeptAddDlg.ui
    modules/dept/DeptEditDlg.h
    modules/dept/DeptEditDlg.cpp
    modules/dept/DeptEditDlg.ui


    modules/post/PostMgrPage.h
    modules/post/PostMgrPage.cpp
    modules/post/PostMgrPage.ui
    modules/post/PostAddDlg.h
    modules/post/PostAddDlg.cpp
    modules/post/PostAddDlg.ui
    modules/post/PostEditDlg.h
    modules/post/PostEditDlg.cpp
    modules/post/PostEditDlg.ui

    domain/JsonMapper.h
    domain/JsonMapper.cpp

    Zc-Manager-client.rc
    Resource.qrc
)
qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE FALSE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        QXlsx::QXlsx
)

