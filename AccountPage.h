﻿#pragma once

#include "AccountPage.g.h"

namespace winrt::bikabika::implementation
{
    struct AccountPage : AccountPageT<AccountPage>
    {
        AccountPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::bikabika::factory_implementation
{
    struct AccountPage : AccountPageT<AccountPage, implementation::AccountPage>
    {
    };
}