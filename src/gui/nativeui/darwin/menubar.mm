/*
 Marmalade - Lightweight Game Engine
 Copyright (C) 2025 Tayler Parsons
 Copyright (C) 2025 Ryan Bester

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if NATIVEUI_ENABLE_MENUBAR

#include "../menubar.h"

#include <Cocoa/Cocoa.h>

#if NATIVEUI_ENABLE_IMGUI
#include <imgui.h>
#include <IconsCodicons.h>
#endif

#include "../../../application/config/engineconfig.h"

#include <iconmapping.h>

using namespace Marmalade::GUI::NativeUI;

@interface MenuCallbackTarget : NSObject
@property (nonatomic, copy) void (^callback)(void);
    - (void)onMenuClick:(id)sender;
@end

@implementation MenuCallbackTarget
    - (void)onMenuClick:(id)sender {
        if (self.callback) self.callback();
    }
@end

struct IconResult {
    std::string cleanLabel;
    std::string assetName;
};

IconResult resolveIconAndCleanLabel(const std::string& input, const std::map<std::string, std::string>& mapping) {
    NSMutableString *nsLabel = [NSMutableString stringWithUTF8String:input.c_str()];
    std::string foundAssetName = "";

    for (auto const& [iconBytes, name] : mapping) {
        NSString *iconStr = [NSString stringWithUTF8String:iconBytes.c_str()];
        NSRange range = [nsLabel rangeOfString:iconStr];

        if (range.location != NSNotFound) {
            foundAssetName = name;
            [nsLabel deleteCharactersInRange:range];
            break;
        }
    }

    NSString *finalNSStr = [nsLabel stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];

    return {
        .cleanLabel = std::string([finalNSStr UTF8String]),
        .assetName = foundAssetName
    };
}

std::stack<void*> MenuBar::_menuStack{};
std::map<std::string, bool> MenuBar::_clickStates{};
std::vector<void*> MenuBar::_menuTargets{};

std::map<std::string, void*> MenuBar::_allMenuItems{};
std::unordered_set<std::string> MenuBar::_itemsSeenThisFrame{};

bool MenuBar::BeginMainMenuBar() {
    _itemsSeenThisFrame.clear();
    return ImGui::BeginMainMenuBar();
}

void MenuBar::EndMainMenuBar() {
    ImGui::EndMainMenuBar();

    // Check for removed items
    auto it = _allMenuItems.begin();
    while (it != _allMenuItems.end()) {
        if (_itemsSeenThisFrame.find(it->first) == _itemsSeenThisFrame.end()) {
            // Menu item no longer exists, remove it
            NSMenuItem* nativeItem = (NSMenuItem*)it->second;
            NSMenu* menu = [nativeItem menu];
            if (menu != nil) {
                [menu removeItem:nativeItem];
            }
            it = _allMenuItems.erase(it);
        } else {
            ++it;
        }
    }
}

bool MenuBar::BeginMenu(const char* label) {
    if (!EngineConfig::GetStoredConfig().appearance.useNativeMenubar) {
        return ImGui::BeginMenu(label);
    }

    NSMenu *currentParent = MenuBar::_menuStack.empty() ? [NSApp mainMenu] : (NSMenu*)_menuStack.top();
    auto iconResult = resolveIconAndCleanLabel(label, s_IconToAssetName);

    NSMenuItem *item = [currentParent itemWithTitle:[NSString stringWithUTF8String:iconResult.cleanLabel.c_str()]];
    if (!item) {
        item = [currentParent addItemWithTitle:[NSString stringWithUTF8String:iconResult.cleanLabel.c_str()] action:nil keyEquivalent:@""];
        NSMenu *newSubmenu = [[NSMenu alloc] initWithTitle:[NSString stringWithUTF8String:iconResult.cleanLabel.c_str()]];
        [newSubmenu setAutoenablesItems:NO];
        [item setSubmenu:newSubmenu];

        if (currentParent == [NSApp mainMenu] && iconResult.cleanLabel == "Window") {
            // Set system window menu
            [NSApp setWindowsMenu:newSubmenu];
        }

        if (!iconResult.assetName.empty()) {
            NSImage *image = [NSImage imageNamed:[NSString stringWithUTF8String:iconResult.assetName.c_str()]];
            if (image) {
                [image setSize:NSMakeSize(16, 16)];
                item.image = image;
            }
        }
    }

    std::string hierKey = buildHierarchyKey(iconResult.cleanLabel);
    _allMenuItems[hierKey] = item;
    _itemsSeenThisFrame.insert(hierKey);

    MenuBar::_menuStack.push((__bridge void*)[item submenu]);
    return true;
}

void MenuBar::EndMenu() {
    if (!EngineConfig::GetStoredConfig().appearance.useNativeMenubar) {
        ImGui::EndMenu();
        return;
    }
    if (!_menuStack.empty()) {_menuStack.pop();}
}

bool MenuBar::MenuItem(const char* label) {
    return MenuBar::MenuItem(label, nullptr, nullptr, true);
}

bool MenuBar::MenuItem(const char *label, const char* shortcut, bool* p_selected, bool enabled) {
    if (!EngineConfig::GetStoredConfig().appearance.useNativeMenubar) {
        return ImGui::MenuItem(label, shortcut, p_selected, enabled);
    }

    std::string key = label;
    auto iconResult = resolveIconAndCleanLabel(label, s_IconToAssetName);

    NSMenu *currentParent = (NSMenu*)MenuBar::_menuStack.top();
    NSMenuItem *item = [currentParent itemWithTitle:[NSString stringWithUTF8String:iconResult.cleanLabel.c_str()]];
    if (!item) {
        MenuCallbackTarget *target = [[MenuCallbackTarget alloc] init];
        target.callback = ^() {
            if (p_selected) {
                *p_selected = !(*p_selected);
            }
            MenuBar::_clickStates[key] = true;
        };
        MenuBar::_menuTargets.push_back((__bridge_retained void*)target);

        NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:[NSString stringWithUTF8String:iconResult.cleanLabel.c_str()]
        action:@selector(onMenuClick:)
        keyEquivalent:@""];

        [item setTarget:target];

        if (!iconResult.assetName.empty()) {
            NSImage *image = [NSImage imageNamed:[NSString stringWithUTF8String:iconResult.assetName.c_str()]];
            if (image) {
                [image setSize:NSMakeSize(16, 16)];
                item.image = image;
            }
        }

        [currentParent addItem:item];
    }

    [item setEnabled:enabled ? YES : NO];
    if (p_selected) {
        [item setState:(*p_selected) ? NSControlStateValueOn : NSControlStateValueOff];
    }

    std::string hierKey = buildHierarchyKey(iconResult.cleanLabel);
    _allMenuItems[hierKey] = item;
    _itemsSeenThisFrame.insert(hierKey);

    if (MenuBar::_clickStates[key]) {
        MenuBar::_clickStates[key] = false;
        return true;
    }

    return false;
}

void MenuBar::Separator() {
    if (!EngineConfig::GetStoredConfig().appearance.useNativeMenubar) {
        ImGui::Separator();
        return;
    }

    if (_menuStack.empty()) return;
    NSMenu *currentParent = (NSMenu*)_menuStack.top();

    NSArray* items = [currentParent itemArray];
    if ([items count] > 0) {
        if ([[items lastObject] isSeparatorItem]) {
            return;
        }
    }

    [currentParent addItem:[NSMenuItem separatorItem]];
}

std::string MenuBar::buildHierarchyKey(std::string currentItem) {
    std::stack<void*> tempStack = _menuStack;
    std::vector<std::string> pathParts{};

    while (!tempStack.empty()) {
        NSMenu *m = (NSMenu*)tempStack.top();
        if (m != nullptr) {
            NSString *titleStr = [m title];
            if (titleStr != nil) {
                pathParts.push_back(std::string([titleStr UTF8String]));
            }
        }

        tempStack.pop();
    }

    std::string finalKey = "";
    for (auto it = pathParts.rbegin(); it != pathParts.rend(); ++it) {
        if (!finalKey.empty()) {
            finalKey += "//";
        }
        finalKey += *it;
    }

    finalKey += "//" + currentItem;

    return finalKey;
}

#endif
