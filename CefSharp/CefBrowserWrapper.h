// Copyright � 2010-2013 The CefSharp Project. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#pragma once

#include "Stdafx.h"
#include "BrowserSettings.h"
#include "MouseButtonType.h"
#include "IRenderWebBrowser.h"
#include "Internals/RenderClientAdapter.h"

using namespace CefSharp::Internals;

namespace CefSharp
{
    public ref class CefBrowserWrapper
    {
    private:
        RenderClientAdapter* _renderClientAdapter;

    public:
        property Object^ BitmapLock
        {
            Object^ get() { return _renderClientAdapter->BitmapLock; }
        }

        property int BitmapWidth
        {
            int get() { return _renderClientAdapter->BitmapWidth; }
        }

        property int BitmapHeight
        {
            int get() { return _renderClientAdapter->BitmapHeight; }
        }

        CefBrowserWrapper(IRenderWebBrowser^ offscreenBrowserControl)
        {
            _renderClientAdapter = new RenderClientAdapter(offscreenBrowserControl);
        }

        ~CefBrowserWrapper()
        {
            _renderClientAdapter = nullptr;
        }

        void CreateOffscreenBrowser(BrowserSettings^ browserSettings, IntPtr^ sourceHandle, Uri^ uri)
        {
            HWND hwnd = static_cast<HWND>(sourceHandle->ToPointer());
            CefWindowInfo window;
            window.SetAsOffScreen(hwnd);
            CefString url = StringUtils::ToNative(uri->ToString());

            CefBrowserHost::CreateBrowser(window, _renderClientAdapter, url,
                *(CefBrowserSettings*) browserSettings->_internalBrowserSettings);
        }

        void Close()
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost != nullptr)
            {
                cefHost->CloseBrowser(true);
            }
        }

        void WasResized()
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost != nullptr)
            {
                cefHost->WasResized();
            }
        }

        void SendFocusEvent(bool isFocused)
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost != nullptr)
            {
                cefHost->SendFocusEvent(isFocused);
            }
        }

        bool SendKeyEvent(int message, int wParam, int lParam)
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost == nullptr)
            {
                return false;
            }
            else
            {
                CefKeyEvent keyEvent;
                if (message == WM_CHAR)
                    keyEvent.type = KEYEVENT_CHAR;
                else if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
                    keyEvent.type = KEYEVENT_KEYDOWN;
                else if (message == WM_KEYUP || message == WM_SYSKEYUP)
                    keyEvent.type = KEYEVENT_KEYUP;

                keyEvent.windows_key_code = keyEvent.native_key_code = wParam;
                keyEvent.is_system_key = 
                    message == WM_SYSKEYDOWN ||
                    message == WM_SYSKEYUP ||
                    message == WM_SYSCHAR;

                cefHost->SendKeyEvent(keyEvent);
            }
        }

        void OnMouseMove(int x, int y, bool mouseLeave)
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost != nullptr)
            {
                CefMouseEvent mouseEvent;
                mouseEvent.x = x;
                mouseEvent.y = y;

                cefHost->SendMouseMoveEvent(mouseEvent, mouseLeave);
            }
        }

        void OnMouseButton(int x, int y, MouseButtonType mouseButtonType, bool mouseUp, int clickCount)
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost != nullptr)
            {
                CefMouseEvent mouseEvent;
                mouseEvent.x = x;
                mouseEvent.y = y;

                cefHost->SendMouseClickEvent(mouseEvent, (CefBrowserHost::MouseButtonType) mouseButtonType, mouseUp, clickCount);
            }
        }

        void OnMouseWheel(int x, int y, int deltaX, int deltaY)
        {
            auto cefHost = _renderClientAdapter->TryGetCefHost();

            if (cefHost != nullptr)
            {
                CefMouseEvent mouseEvent;
                mouseEvent.x = x;
                mouseEvent.y = y;

                cefHost->SendMouseWheelEvent(mouseEvent, deltaX, deltaY);
            }
        }
    };
}