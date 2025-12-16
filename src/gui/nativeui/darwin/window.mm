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

#include "../window.h"

#include <Cocoa/Cocoa.h>

bool Marmalade::GUI::NativeUI::Window::Create(bool borderless) {
    NSRect screen_rect = [[NSScreen mainScreen] frame];
    NSRect window_rect = NSMakeRect(0, 0, _width, _height);

    NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                  NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    if (borderless) styleMask = NSWindowStyleMaskBorderless;

    NSWindow* win = [[NSWindow alloc] initWithContentRect:window_rect
                                                styleMask:styleMask
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];
    [win retain];

    [win setTitle:Marmalade::GUI::NativeUI::Util::utf16ToPlatformStr(_title)];
    [win center];
    [win setReleasedWhenClosed:NO];

    this->_handle = new DarwinObjWrapper<NSWindow*>(win);

    if (_createCallback) _createCallback();

    return true;
}

Marmalade::GUI::NativeUI::Window::~Window() {
    delete this->_handle;
}

bool Marmalade::GUI::NativeUI::Window::Show(bool topmost) {
    [this->_handle->GetObject() makeKeyAndOrderFront:nil];
    return true;
}

void Marmalade::GUI::NativeUI::Window::Close() {
    if (this->_handle == nullptr) {
        NSLog(@"Handle is null");
        return;
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        [this->_handle->GetObject() close];
    });
}
