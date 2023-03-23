#pragma once

#include "MainWindow.g.h"

#include <windows.h>

#include <string>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.Graphics.Canvas.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>

#include "fpscounter.h"
#include "Scene.h"
#include "Renderer.h"

namespace winrt::Butternut::implementation
{
    enum class PointerMode
    {
        Left,
        Middle,
        Right,
        None
    };

    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        // construction and initialization
        MainWindow() = default;
        void InitializeComponent();

        // standard window stuff
        void OnWindowActivate(IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void OnWindowResized(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowSizeChangedEventArgs const& args) noexcept;
        void OnWindowClosed(IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args) noexcept;
        void InvalidateIfNeeded();
        void SetMyTitleBar();

        // starting the game and handling the timer
        void StartGameLoop();

        void PumpProperties();

        // drawing stats
        //void CanvasStats_Draw(Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);

        // property change handlers
        void OnPropertyChanged(IInspectable const& sender, PropertyChangedEventArgs const& args);
        winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
        {
            return _propertyChanged.add(value);
        }

        void PropertyChanged(winrt::event_token const& token)
        {
            _propertyChanged.remove(token);
        }

        // xaml bindings
        winrt::hstring StatusMain() const;
        winrt::hstring FPSAverage() const;

        // event handlers
        void OnTick(winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer const&, IInspectable const&);

        void OnKeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void OnPointerPressed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void OnPointerMoved(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void OnPointerReleased(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) noexcept;
        void OnPointerExited(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) noexcept;
        void CanvasBoard_SizeChanged(IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void GoButton_Click(IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ResetButton_Click(IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        winrt::fire_and_forget ShowMessageBox(const winrt::hstring& title, const winrt::hstring& message);

        void SetStatus(const std::string& message);
        void CanvasBoard_CreateResources(Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender, winrt::Microsoft::Graphics::Canvas::UI::CanvasCreateResourcesEventArgs const& args);
        HWND GetWindowHandle() const;
        void OnDPIChanged();
        void OnCanvasDeviceChanged();
        void SetBestCanvasandWindowSizes();
        void CanvasBoard_Draw(Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);
        void OnFirstRun();
        int ConvertToPixels(float dips);

    private:
        Microsoft::Graphics::Canvas::CanvasDevice _canvasDevice{ nullptr };
        Timer _frametimer;
        Renderer _renderer;
        Scene _scene;

        FPScounter fps{};
        float _dpi{ 0.0f };
        float _lastFrameTime{ 0 };

        PointerMode _PointerMode = PointerMode::None;
        winrt::Windows::System::VirtualKey _key;

        bool _closing = false;

        winrt::Microsoft::UI::Dispatching::DispatcherQueueController _controller{ nullptr };
        winrt::Microsoft::UI::Dispatching::DispatcherQueue _queue{ nullptr };
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer timer{ nullptr };
        winrt::event_token _eventtoken{ 0 };

        winrt::event_token _propertyToken;
        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
        std::string _statusMain{ "Ready" };
    };
}

namespace winrt::Butternut::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}