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

std::stack<void*> MenuBar::_menuStack{};
std::map<std::string, bool> MenuBar::_clickStates{};
std::map<std::string, void*> MenuBar::_nativeItems{};
std::vector<void*> MenuBar::_menuTargets{};

static std::map<std::string, std::string> s_IconToSFSymbol = {
    {ICON_CI_ADD, "doc.badge.plus"},
    {ICON_CI_BLANK, ""}
};

bool MenuBar::BeginMainMenuBar() {
    return ImGui::BeginMainMenuBar();
}

void MenuBar::EndMainMenuBar() {
    ImGui::EndMainMenuBar();
}

bool MenuBar::BeginMenu(const char* label) {
    if (!EngineConfig::GetStoredConfig().appearance.useNativeMenubar) {
        return ImGui::BeginMenu(label);
    }

    NSMenu *currentParent = MenuBar::_menuStack.empty() ? [NSApp mainMenu] : (NSMenu*)_menuStack.top();
    NSString *title = [NSString stringWithUTF8String:label];

    NSMenuItem *item = [currentParent itemWithTitle:title];
    if (!item) {
        item = [currentParent addItemWithTitle:title action:nil keyEquivalent:@""];
        NSMenu *newSubmenu = [[NSMenu alloc] initWithTitle:title];
        [newSubmenu setAutoenablesItems:NO];
        [item setSubmenu:newSubmenu];
    }

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
    NSMutableString* nsLabel = [NSMutableString stringWithUTF8String:label];

    // Replace unicode font symbols with SF symbols
    NSString* symbolName = nil;
    for (auto const& [icon, symbol] : s_IconToSFSymbol) {
        NSRange range = [nsLabel rangeOfString:[NSString stringWithUTF8String:icon.c_str()]];
        if (range.location != NSNotFound) {
            symbolName = [NSString stringWithUTF8String:symbol.c_str()];
            [nsLabel deleteCharactersInRange:range];
            break;
        }
    }

    // Remove unknown unicode font symbols
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"[\\uE000-\\uF8FF]"
                                                                           options:0
                                                                             error:nil];
    [regex replaceMatchesInString:nsLabel
                          options:0
                            range:NSMakeRange(0, [nsLabel length])
                     withTemplate:@""];

    NSString* cleanTitle = [nsLabel stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];

    NSMenu *currentParent = (NSMenu*)MenuBar::_menuStack.top();
    NSMenuItem *item = [currentParent itemWithTitle:cleanTitle];
    if (!item) {
        MenuCallbackTarget *target = [[MenuCallbackTarget alloc] init];
        target.callback = ^() {
            if (p_selected) {
                *p_selected = !(*p_selected);
            }
            MenuBar::_clickStates[key] = true;
        };
        MenuBar::_menuTargets.push_back((__bridge_retained void*)target);

        NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:cleanTitle
        action:@selector(onMenuClick:)
        keyEquivalent:@""];

        [item setTarget:target];

        if (symbolName) {
            if (@available(macOS 11.0, *)) {
                item.image = [NSImage imageWithSystemSymbolName:symbolName
                                         accessibilityDescription:nil];
            }
        }

        [currentParent addItem:item];
    }

    [item setEnabled:enabled ? YES : NO];
    if (p_selected) {
        [item setState:(*p_selected) ? NSControlStateValueOn : NSControlStateValueOff];
    }

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

#endif
