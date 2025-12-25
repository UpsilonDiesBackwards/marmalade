import Cocoa

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        createDefaultMenuBar()

        window = NSWindow(
            contentRect: NSRect(x: 100, y: 100, width: 600, height: 400),
            styleMask: [.titled, .closable, .resizable, .miniaturizable],
            backing: .buffered,
            defer: false
        )
        window.title = "Marmalade Crash Reporter"
        window.makeKeyAndOrderFront(nil)

        NSApp.setActivationPolicy(.regular)
        NSApp.activate(ignoringOtherApps: true)
    }

    func createDefaultMenuBar() {
        let mainMenu = NSMenu()

        let appMenuItem = NSMenuItem()
        mainMenu.addItem(appMenuItem)

        let appMenu = NSMenu()

        appMenu.addItem(
            withTitle: "About Marmalade Crash Reporter",
            action: #selector(NSApplication.orderFrontStandardAboutPanel(_:)),
            keyEquivalent: ""
        )

        appMenu.addItem(NSMenuItem.separator())

        let servicesMenu = NSMenu(title: "Services")
        NSApp.servicesMenu = servicesMenu

        let servicesItem = NSMenuItem(
            title: "Services",
            action: nil,
            keyEquivalent: ""
        )
        servicesItem.submenu = servicesMenu
        appMenu.addItem(servicesItem)

        appMenu.addItem(NSMenuItem.separator())

        appMenu.addItem(
            withTitle: "Hide Marmalade Crash Reporter",
            action: #selector(NSApplication.hide(_:)),
            keyEquivalent: "h"
        )

        let hideOthersItem = appMenu.addItem(
            withTitle: "Hide Others",
            action: #selector(NSApplication.hideOtherApplications(_:)),
            keyEquivalent: "h"
        )
        hideOthersItem.keyEquivalentModifierMask = [.command, .option]

        appMenu.addItem(
            withTitle: "Show All",
            action: #selector(NSApplication.unhideAllApplications(_:)),
            keyEquivalent: ""
        )

        appMenu.addItem(NSMenuItem.separator())

        appMenu.addItem(
            withTitle: "Quit Marmalade Crash Reporter",
            action: #selector(NSApplication.terminate(_:)),
            keyEquivalent: "q"
        )

        appMenuItem.submenu = appMenu

        let fileMenuItem = NSMenuItem()
        mainMenu.addItem(fileMenuItem)

        let fileMenu = NSMenu(title: "File")

        fileMenuItem.submenu = fileMenu

        let helpMenuItem = NSMenuItem()
        mainMenu.addItem(helpMenuItem)

        let helpMenu = NSMenu(title: "Help")
        helpMenuItem.submenu = helpMenu
        NSApp.helpMenu = helpMenu

        NSApp.mainMenu = mainMenu
    }

}

let delegate = AppDelegate()
NSApplication.shared.delegate = delegate
NSApplication.shared.setActivationPolicy(.regular)
NSApplication.shared.run()
