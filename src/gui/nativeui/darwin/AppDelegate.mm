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

#import "AppDelegate.h"

@interface AppDelegate () {
    std::function<void()> launchCallback;
}
@end

@implementation AppDelegate

- (void)setLaunchCallback:(std::function<void()>)callback {
    launchCallback = std::move(callback);
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
    [NSApp activateIgnoringOtherApps:YES];

    [self createMenu];

    launchCallback();
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

- (void)addSeparator:(NSMenu*)menu {
    NSMenuItem *separator = [NSMenuItem separatorItem];
    [menu addItem:separator];
}

- (void)addMenuItem:(NSMenu*)menu title:(nonnull NSString*)title action:(nullable SEL)selector keyEquivalent:(nonnull NSString*)charCode {
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:title
                                                       action:selector
                                                keyEquivalent:charCode];
    [menu addItem:item];
}

- (void)addMenuItem:(NSMenu*)menu title:(nonnull NSString*)title action:(nullable SEL)selector keyEquivalent:(nonnull NSString*)charCode modifierMask:(NSEventModifierFlags)mask {
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:title
                                                       action:selector
                                                keyEquivalent:charCode];
    [item setKeyEquivalentModifierMask:mask];
    [menu addItem:item];
}

- (void)addAppMenu:(NSMenu*)mainMenu {
    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];

    [self.mainMenu addItem:appMenuItem];

    NSMenu* appMenu = [[NSMenu alloc] init];

    [self addMenuItem:appMenu title:@"About Marmalade Engine" action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
    [self addSeparator:appMenu];

    [self addMenuItem:appMenu title:@"Preferences..." action:@selector(openPreferences:) keyEquivalent:@","];
    [self addSeparator:appMenu];

    NSMenuItem* servicesItem = [appMenu addItemWithTitle:@"Services" action:nil keyEquivalent:@""];
    NSMenu* servicesMenu = [[NSMenu alloc] initWithTitle:@"Services"];
    [servicesItem setSubmenu:servicesMenu];
    [NSApp setServicesMenu:servicesMenu];
    [self addSeparator:appMenu];

    [self addMenuItem:appMenu title:@"Hide Marmalade Engine" action:@selector(hide:) keyEquivalent:@"h"];
    [self addMenuItem:appMenu title:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h" modifierMask:NSEventModifierFlagCommand | NSEventModifierFlagOption];
    [self addMenuItem:appMenu title:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

    [self addSeparator:appMenu];

    [self addMenuItem:appMenu title:@"Quit Marmalade Engine" action:@selector(terminate:) keyEquivalent:@"q"];

    [appMenuItem setSubmenu:appMenu];
}

- (void)createMenu {
    self.mainMenu = [[NSMenu alloc] init];
    [NSApp setMainMenu:self.mainMenu];

    [self addAppMenu:self.mainMenu];
}

@end
