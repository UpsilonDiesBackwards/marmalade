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

    launchCallback();

    [self createMenu];
}

- (void)applicationWillTerminate:(NSNotification*)aNotification {
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

- (void)createMenu {
    self.mainMenu = [[NSMenu alloc] init];

    NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];

    [self.mainMenu addItem:appMenuItem];
    [NSApp setMainMenu:self.mainMenu];

    NSMenu* appMenu = [[NSMenu alloc] init];
    NSString* appName = [[NSProcessInfo processInfo] processName];

    NSMenuItem* aboutItem = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"About %@", appName]
                                                       action:@selector(orderFrontStandardAboutPanel:)
                                                keyEquivalent:@""];

    NSMenuItem* prefsItem = [[NSMenuItem alloc] initWithTitle:@"Preferences..."
                                                       action:@selector(openPreferences:)
                                                keyEquivalent:@","];

    NSMenuItem* separator = [NSMenuItem separatorItem];

    NSMenuItem* quitItem = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
                                                      action:@selector(terminate:)
                                               keyEquivalent:@"q"];

    [appMenu addItem:aboutItem];
    [appMenu addItem:prefsItem];
    [appMenu addItem:separator];
    [appMenu addItem:quitItem];

    [appMenuItem setSubmenu:appMenu];
}

@end
