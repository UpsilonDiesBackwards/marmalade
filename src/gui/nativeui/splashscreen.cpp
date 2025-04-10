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

#include "splashscreen.h"

#include "util.h"

#ifdef _WIN32

#include <windows.h>

#elif __APPLE__

#else

#include <gtk-3.0/gtk/gtk.h>

#endif

using namespace Marmalade::GUI;

#define TEXTURE_PATH "res/textures/splash"

std::string NativeUI::SplashScreen::_loadingText = "";

#ifdef _WIN32

HBITMAP backgroundBmp = nullptr;
HWND loadingTextWnd = nullptr;

void loadBackgroundImage(HWND wnd) {
    backgroundBmp = (HBITMAP) LoadImage(
            nullptr,
            TEXT(TEXTURE_PATH) L".bmp",
            IMAGE_BITMAP,
            0, 0,
            LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!backgroundBmp) {
        MessageBox(wnd, L"Error loading image", L"Error", MB_ICONERROR);
    }
}

void drawBackground(HDC dc, RECT& rect) {
    if (!backgroundBmp) return;

    HDC dcMem = CreateCompatibleDC(dc);
    HGDIOBJ oldBitmap = SelectObject(dcMem, backgroundBmp);

    BITMAP bitmap;
    GetObject(backgroundBmp, sizeof(BITMAP), &bitmap);

    StretchBlt(dc, 0, 0, rect.right, rect.bottom,
               dcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
    SelectObject(dcMem, oldBitmap);
    DeleteDC(dcMem);
}

void createInternal(NativeUI::Window& window) {
    loadBackgroundImage(window.GetHandle());

    auto u16 = NativeUI::Util::utf8ToUtf16Str(NativeUI::SplashScreen::GetLoadingText());
    auto platform = NativeUI::Util::utf16ToPlatformStr(u16);

    loadingTextWnd = CreateWindow(L"STATIC", platform, WS_CHILD | WS_VISIBLE, 450, 390, 200, 23, window.GetHandle(), nullptr, nullptr, nullptr);
}

void paintInternal(NativeUI::Window& window) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(window.GetHandle(), &ps);

    RECT rect;
    GetClientRect(window.GetHandle(), &rect);
    drawBackground(dc, rect);

    EndPaint(window.GetHandle(), &ps);
}

void updateLoadingTextInternal(NativeUI::Window& window) {
    if (loadingTextWnd != nullptr) {
        auto u16 = NativeUI::Util::utf8ToUtf16Str(NativeUI::SplashScreen::GetLoadingText());
        auto platform = NativeUI::Util::utf16ToPlatformStr(u16);
        SetWindowText(loadingTextWnd, platform);
    }
}

#elif __APPLE__

@interface SplashImageView : NSView

@property(nonatomic, strong) NSString* loadingText;

- (void)updateLoadingText:(NSString*)text;

@end

@implementation SplashImageView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];

    NSString* imagePath = [[NSBundle mainBundle] pathForResource:@TEXTURE_PATH ofType:@"png"];
    NSImage* image = [[NSImage alloc] initWithContentsOfFile:imagePath];

    if (!image) {
        NSLog(@"Failed to load image at %@", imagePath);
        return;
    }

    NSSize imageSize = [image size];
    NSSize viewSize = [self bounds].size;

    NSRect targetRect = NSMakeRect(0, 0, viewSize.width, viewSize.height);

    [[NSGraphicsContext currentContext] saveGraphicsState];

    [image drawInRect:targetRect
             fromRect:NSZeroRect
            operation:NSCompositingOperationSourceOver
             fraction:1.0];

    NSDictionary* attributes = @{
        NSFontAttributeName: [NSFont systemFontOfSize:20],
        NSForegroundColorAttributeName: [NSColor blackColor]
    };

    NSRect textRect = NSMakeRect(450, 390, self.bounds.size.width - 450, 30);
    [self.loadingText drawInRect:textRect withAttributes:attributes];

    [[NSGraphicsContext currentContext] restoreGraphicsState];
}

- (void)updateLoadingText:(NSString*)text {
    self.loadingText = text;
    [self setNeedsDisplay:YES];
}

@end

SplashImageView* imageView;

void createInternal(NativeUI::Window& window) {
    NSRect window_rect = NSMakeRect(0, 0, window.GetWidth(), window.GetHeight());

    NSViewController* viewController = [[NSViewController alloc] init];
    viewController.view = [[NSView alloc] initWithFrame:window_rect];
    viewController.view.wantsLayer = YES;
    viewController.view.layer.backgroundColor = [[NSColor grayColor] CGColor];

    auto win = window.GetHandle()->GetObject();
    win.contentViewController = viewController;

    imageView = [[SplashImageView alloc] initWithFrame:win.contentView.bounds];
    [win.contentView addSubview:imageView];
}

void paintInternal(NativeUI::Window& window) {}

void updateLoadingTextInternal(NativeUI::Window& window) {
    [imageView updateLoadingText:[NSString stringWithUTF8String:NativeUI::SplashScreen::GetLoadingText().c_str()]];
}

#else

GtkWidget* label = nullptr;

gboolean onDraw(GtkWidget* widget, cairo_t* cr, gpointer userData) {
    cairo_surface_t* imageSurface = cairo_image_surface_create_from_png(TEXTURE_PATH ".png");

    if (cairo_surface_status(imageSurface) != CAIRO_STATUS_SUCCESS) {
        g_print("Failed to load image\n");
        return FALSE;
    }

    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    int imgWidth = cairo_image_surface_get_width(imageSurface);
    int imgHeight = cairo_image_surface_get_height(imageSurface);

    cairo_scale(cr, (double) width / imgWidth, (double) height / imgHeight);

    cairo_set_source_surface(cr, imageSurface, 0, 0);
    cairo_paint(cr);

    cairo_surface_destroy(imageSurface);

    return FALSE;
}

void createInternal(NativeUI::Window& window) {
    GtkWidget* fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window.GetHandle()), fixed);

    GtkWidget* drawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingArea, window.GetWidth(), window.GetHeight());
    gtk_fixed_put(GTK_FIXED(fixed), drawingArea, 0, 0);

    g_signal_connect(drawingArea, "draw", G_CALLBACK(onDraw), nullptr);

    label = gtk_label_new(NativeUI::SplashScreen::GetLoadingText().c_str());

    gtk_fixed_put(GTK_FIXED(fixed), label, 450, 390);
}

void paintInternal(NativeUI::Window& window) {}

void updateLoadingTextInternal(NativeUI::Window& window) {
    if (label != nullptr) {
        gtk_label_set_text(GTK_LABEL(label), NativeUI::SplashScreen::GetLoadingText().c_str());
    }
}

#endif

void NativeUI::SplashScreen::Create(NativeUI::Window& window) {
    createInternal(window);
}

void NativeUI::SplashScreen::Paint(NativeUI::Window& window) {
    paintInternal(window);
}

void NativeUI::SplashScreen::updateLoadingText(NativeUI::Window& window) {
    updateLoadingTextInternal(window);
}

bool NativeUI::SplashScreen::AreSafeModeKeysHeld() {
#ifdef _WIN32
    bool isLeftCtrlDown = GetAsyncKeyState(VK_LCONTROL) & 0x8000;
    bool isLeftShiftDown = GetAsyncKeyState(VK_LSHIFT) & 0x8000;
#elif __APPLE__
    bool isLeftCtrlDown = (NSEvent.modifierFlags & NSEventModifierFlagControl) != 0;
    bool isLeftShiftDown = (NSEvent.modifierFlags & NSEventModifierFlagShift) != 0;
#else
    GdkDisplay* display = gdk_display_get_default();
    gboolean isLeftCtrlDown = gdk_keymap_get_modifier_state(gdk_keymap_get_for_display(display)) & GDK_CONTROL_MASK;
    gboolean isLeftShiftDown = gdk_keymap_get_modifier_state(gdk_keymap_get_for_display(display)) & GDK_SHIFT_MASK;
#endif

    return isLeftCtrlDown && isLeftShiftDown;
}
