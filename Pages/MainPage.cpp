﻿#include "pch.h"

#include "MainPage.h"

#include "MainPage.g.cpp"




using namespace std;
using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Composition;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;

namespace winrt::bikabika::implementation
{

	
	MainPage::MainPage()
	{
		
		InitializeComponent();
		Windows::Storage::ApplicationDataContainer serversSettings = Windows::Storage::ApplicationData::Current().LocalSettings().CreateContainer(L"Servers", Windows::Storage::ApplicationDataCreateDisposition::Always);
		Windows::Storage::ApplicationDataContainer userData = Windows::Storage::ApplicationData::Current().LocalSettings().CreateContainer(L"User", Windows::Storage::ApplicationDataCreateDisposition::Always);
		if (!serversSettings.Values().HasKey(L"picServer1")) serversSettings.Values().Insert(L"picServer1", box_value(L"https://storage1.picacomic.com"));
		if (!serversSettings.Values().HasKey(L"picServer2")) serversSettings.Values().Insert(L"picServer2", box_value(L"https://s2.picacomic.com"));
		if (!serversSettings.Values().HasKey(L"picServer3")) serversSettings.Values().Insert(L"picServer3", box_value(L"https://s3.picacomic.com"));
		if (!serversSettings.Values().HasKey(L"picServerCurrent")) serversSettings.Values().Insert(L"picServerCurrent", box_value(L"https://storage1.picacomic.com"));
		Windows::Storage::ApplicationDataContainer localSettings = Windows::Storage::ApplicationData::Current().LocalSettings();
		if (!localSettings.Values().HasKey(L"launchedWithPrefSize"))
		{
			ApplicationView::GetForCurrentView().PreferredLaunchViewSize(Size(1470, 1000));
			ApplicationView::GetForCurrentView().SetPreferredMinSize(Size(1470, 1000));
			ApplicationView::GetForCurrentView().PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);
			localSettings.Values().Insert(L"launchedWithPrefSize", box_value(true));
		}
		ApplicationView::GetForCurrentView().PreferredLaunchWindowingMode(ApplicationViewWindowingMode::Auto);
		
		
		m_pages.push_back(std::make_pair<std::wstring, Windows::UI::Xaml::Interop::TypeName>
			(L"home", winrt::xaml_typename<bikabika::HomePage>()));
		m_pages.push_back(std::make_pair<std::wstring, Windows::UI::Xaml::Interop::TypeName>
			(L"classification", winrt::xaml_typename<bikabika::ClassificationPage>()));
		m_pages.push_back(std::make_pair<std::wstring, Windows::UI::Xaml::Interop::TypeName>
			(L"account", winrt::xaml_typename<bikabika::UserPage>()));
		m_pages.push_back(std::make_pair<std::wstring, Windows::UI::Xaml::Interop::TypeName>
			(L"settings", winrt::xaml_typename<bikabika::SettingsPage>()));
		m_pages.push_back(std::make_pair<std::wstring, Windows::UI::Xaml::Interop::TypeName>
			(L"login", winrt::xaml_typename<bikabika::LoginPage>()));
		//NavigationCacheMode(Windows::UI::Xaml::Navigation::NavigationCacheMode::Enabled);
		Window::Current().SetTitleBar(AppTitleBar());
		auto acc{ UpdateToken() };
	}





	
	void MainPage::NavView_Loaded(
		Windows::Foundation::IInspectable const& /* sender */,
		Windows::UI::Xaml::RoutedEventArgs const& /* args */)
	{
		NavView_Navigate(L"login",Windows::UI::Xaml::Media::Animation::EntranceNavigationTransitionInfo());
	}

	void MainPage::NavView_ItemInvoked(
		Windows::Foundation::IInspectable const& /* sender */,
		muxc::NavigationViewItemInvokedEventArgs const& args)
	{
		if (args.IsSettingsInvoked())
		{
			NavView_Navigate(L"settings", args.RecommendedNavigationTransitionInfo());
		}
		else if (args.InvokedItemContainer())
		{
			NavView_Navigate(
				winrt::unbox_value_or<winrt::hstring>(
					args.InvokedItemContainer().Tag(), L"").c_str(),
				args.RecommendedNavigationTransitionInfo());
		}
	}



	void MainPage::NavView_Navigate(
		std::wstring navItemTag,
		Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo const& transitionInfo)
	{
		Windows::UI::Xaml::Interop::TypeName pageTypeName;
		if (navItemTag == L"settings")
		{
			pageTypeName = winrt::xaml_typename<bikabika::SettingsPage>();
		}
		else
		{
			for (auto&& eachPage : m_pages)
			{
				if (eachPage.first == navItemTag)
				{
					pageTypeName = eachPage.second;
					break;
				}
			}
		}
		// Get the page type before navigation so you can prevent duplicate
		// entries in the backstack.
		Windows::UI::Xaml::Interop::TypeName preNavPageType =
			ContentFrame().CurrentSourcePageType();
		
		// Navigate only if the selected page isn't currently loaded.
		if (pageTypeName.Name != L"" && preNavPageType.Name != pageTypeName.Name)
		{
			ContentFrame().Navigate(pageTypeName, nullptr, transitionInfo);
		}
	}

	void MainPage::NavView_BackRequested(
		muxc::NavigationView const& /* sender */,
		muxc::NavigationViewBackRequestedEventArgs const& /* args */)
	{
		TryGoBack();
	}

	void MainPage::CoreDispatcher_AcceleratorKeyActivated(
		Windows::UI::Core::CoreDispatcher const& /* sender */,
		Windows::UI::Core::AcceleratorKeyEventArgs const& args)
	{
		// When Alt+Left are pressed navigate back
		if (args.EventType() == Windows::UI::Core::CoreAcceleratorKeyEventType::SystemKeyDown
			&& args.VirtualKey() == Windows::System::VirtualKey::Left
			&& args.KeyStatus().IsMenuKeyDown
			&& !args.Handled())
		{
			args.Handled(TryGoBack());
		}
	}

	void MainPage::CoreWindow_PointerPressed(
		Windows::UI::Core::CoreWindow const& /* sender */,
		Windows::UI::Core::PointerEventArgs const& args)
	{

		// Handle mouse back button.
		if (args.CurrentPoint().Properties().IsXButton1Pressed())
		{
			args.Handled(TryGoBack());
		}
	}

	void MainPage::System_BackRequested(
		Windows::Foundation::IInspectable const& /* sender */,
		Windows::UI::Core::BackRequestedEventArgs const& args)
	{
		if (!args.Handled())
		{
			args.Handled(TryGoBack());
		}
	}
	void MainPage::ContentFrame_NavigationFailed(
		Windows::Foundation::IInspectable const& /* sender */,
		Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const& args)
	{
		throw winrt::hresult_error(
			E_FAIL, winrt::hstring(L"Failed to load Page ") + args.SourcePageType().Name);
	}
	bool MainPage::TryGoBack()
	{
		if (!ContentFrame().CanGoBack())
			return false;
		// Don't go back if the nav pane is overlayed.
		if (NavView().IsPaneOpen() &&
			(NavView().DisplayMode() == muxc::NavigationViewDisplayMode::Compact ||
				NavView().DisplayMode() == muxc::NavigationViewDisplayMode::Minimal))
			return false;
		ContentFrame().GoBack();
		return true;
	}

	void MainPage::On_Navigated(
		Windows::Foundation::IInspectable const& /* sender */,
		Windows::UI::Xaml::Navigation::NavigationEventArgs const& args)
	{
		NavView().IsBackEnabled(ContentFrame().CanGoBack());

		if (ContentFrame().SourcePageType().Name ==
			winrt::xaml_typename<bikabika::SettingsPage>().Name)
		{
			// SettingsItem is not part of NavView.MenuItems, and doesn't have a Tag.
			NavView().SelectedItem(NavView().SettingsItem().as<muxc::NavigationViewItem>());
			NavView().Header(winrt::box_value(L"Settings"));
		}
		else if (ContentFrame().SourcePageType().Name != L"")
		{
			//m_userViewModel.User().Img(winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage{ Windows::Foundation::Uri{ L"https://storage1.picacomic.com/static/0788a77a-81e4-46a5-9206-c424226bed07.jpg" } });
			for (auto&& eachPage : m_pages)
			{
				if (eachPage.second.Name == args.SourcePageType().Name)
				{
					for (auto&& eachMenuItem : NavView().MenuItems())
					{
						auto navigationViewItem =
							eachMenuItem.try_as<muxc::NavigationViewItem>();
						{
							if (navigationViewItem)
							{
								winrt::hstring hstringValue =
									winrt::unbox_value_or<winrt::hstring>(
										navigationViewItem.Tag(), L"");
								if (hstringValue == eachPage.first)
								{
									NavView().SelectedItem(navigationViewItem);
									NavView().Header(navigationViewItem.Content());
								}
							}
						}
					}
					break;
				}
			}
		}
	}

	bikabika::UserViewModel MainPage::MainUserViewModel()
	{
		return m_userViewModel;
	}

	Windows::Foundation::IAsyncAction MainPage::UpdateToken()
	{
		bool f{ co_await m_fileCheckTool.CheckFileAccount() };
		if (f) {
			Windows::Data::Json::JsonObject account{ co_await m_fileCheckTool.GetAccount() };
			extern winrt::hstring token;
			token = account.GetNamedString(L"token");
			OutputDebugStringW(L"\n自动加载token成功->\n");
			OutputDebugStringW(token.c_str());
			OutputDebugStringW(L"\n");
		}
		
	}
	Windows::Foundation::IAsyncAction MainPage::UpdateSuggestion()
	{
		bool flag{ co_await m_fileCheckTool.CheckFileKeywords() };
		if (flag) {
			m_suggestions.Clear();

			Windows::Data::Json::JsonObject keywords{ co_await m_fileCheckTool.GetKeywords() };
			auto keyword = keywords.GetNamedArray(L"keywords");
			OutputDebugStringW(keyword.ToString().c_str());
			OutputDebugStringW(L"\n");
			for (auto x : keyword)
			{
				m_suggestions.Append(winrt::make<KeywordsBox>(x.GetString(), L"大家都在搜", L"[TAG]", winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage(winrt::Windows::Foundation::Uri(L"ms-appx:///Assets//Picacgs//tag.png"))));
			}

		}
	}

	/*
	* void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
		auto propertyValue{ e.Parameter().as<Windows::Foundation::IPropertyValue>() };
		if (propertyValue.Type() == Windows::Foundation::PropertyType::String)
		{
			auto ss = winrt::unbox_value<winrt::hstring>(e.Parameter());
			Windows::Data::Json::JsonObject personData = Windows::Data::Json::JsonObject::Parse(ss);
			Uri url(L"https://storage1.picacomic.com/static/" + personData.GetNamedObject(L"avatar").GetNamedString(L"path"));
			Windows::UI::Xaml::Media::Imaging::BitmapImage source(url);
			UsersPic().ProfilePicture(source);
		}
		else
		{
			__super::OnNavigatedTo(e);
		}

	}
	Windows::Foundation::IAsyncAction MainPage::CreateLoginPage(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args)
	{	// 新建页面

		auto Views = CoreApplication::Views();
		if (Views.Size() == 2)
			return;
		CoreDispatcher MainDispatcher =
			CoreApplication::GetCurrentView().Dispatcher();
		int32_t  ViewId = 0;
		// Create a new window.

		CoreApplicationView View = CoreApplication::CreateNewView();
		co_await resume_foreground(View.Dispatcher());

		// Show the "Loading..." screen.
		Windows::UI::Xaml::Controls::Frame f;
		f.Navigate(xaml_typename<bikabika::LoginPage>(), nullptr);
		Window::Current().Content(f);
		Window::Current().Activate();

		//ApplicationView::PreferredLaunchViewSize();
		//ApplicationView::PreferredLaunchWindowingMode(ApplicationViewWindowingMode::Auto);
		//Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().TryResizeView(Size(600, 400));

		ViewId = ApplicationView::GetForCurrentView().Id();

		// Activate the new window.
		co_await resume_foreground(MainDispatcher);

		co_await ApplicationViewSwitcher::TryShowAsStandaloneAsync(ViewId);

		// Start the emulation in the new window.
		co_await resume_foreground(View.Dispatcher());
		co_await ApplicationViewSwitcher::SwitchAsync(ViewId);

		//ApplicationView::GetForCurrentView().ExitFullScreenMode();
	}*/

	void winrt::bikabika::implementation::MainPage::ContentFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
		Windows::Storage::ApplicationDataContainer serversSettings = Windows::Storage::ApplicationData::Current().LocalSettings().CreateContainer(L"Servers", Windows::Storage::ApplicationDataCreateDisposition::Always);
		Windows::Storage::ApplicationDataContainer userData = Windows::Storage::ApplicationData::Current().LocalSettings().CreateContainer(L"User", Windows::Storage::ApplicationDataCreateDisposition::Always);

		if (userData.Values().HasKey(L"personInfo"))
		{
			Windows::Data::Json::JsonObject personInfo = winrt::Windows::Data::Json::JsonObject::Parse(unbox_value<winrt::hstring>(userData.Values().TryLookup(L"personInfo")));
			hstring name = personInfo.GetNamedString(L"name");
			if (name != m_userViewModel.User().Name()) m_userViewModel.User().Name(name);
			hstring level = L"Lv." + to_hstring(personInfo.GetNamedNumber(L"level"));
			if (level != m_userViewModel.User().Level()) m_userViewModel.User().Level(level);
			hstring img = unbox_value<winrt::hstring>(serversSettings.Values().Lookup(L"picServersCurrent")) + L"/static/" + personInfo.GetNamedObject(L"avatar").GetNamedString(L"path");
			if (img != m_userViewModel.User().Img().UriSource().ToString()) {
				m_userViewModel.User().Img(winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage{ Windows::Foundation::Uri{ img} });
			}
		}
		extern bool keywordLoaded;
		if (!keywordLoaded) {
			auto bcc{ UpdateSuggestion() };
			keywordLoaded = true;
		}
	}
	void  winrt::bikabika::implementation::MainPage::CatSearch_TextChanged(winrt::Windows::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const& args)
	{
		if (!m_suggestIsChosen)
		{
			
			//suggestions.Append(L"1");
			//suggestions.Append(L"2");
			//suggestions.Append(L"3");
			//auto suggestions = winrt::single_threaded_observable_vector<bikabika::SuggestBlock>();
			//suggestions.Append(winrt::make<KeywordsBox>(L"标签标签标签标签", L"大家都在搜", L"[TAG]", winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage(winrt::Windows::Foundation::Uri(L"ms-appx:///tag.png"))));
			//suggestions.Append(winrt::make<SuggestBox>(L"33333"));
			OutputDebugStringW(L"\n\n\n666\n\n\n");
			sender.ItemsSource(box_value(m_suggestions));

		}
		m_suggestIsChosen = false;
	}
	
	void winrt::bikabika::implementation::MainPage::CatSearch_QuerySubmitted(winrt::Windows::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args)
	{
		extern bool loadComicFlag;
		loadComicFlag = true;
		ContentFrame().Navigate(winrt::xaml_typename<bikabika::ComicsPage>(), box_value(single_threaded_vector<hstring>({ L"Search",sender.Text(), to_hstring("dd") })));
	}


	void winrt::bikabika::implementation::MainPage::CatSearch_SuggestionChosen(winrt::Windows::UI::Xaml::Controls::AutoSuggestBox const& sender, winrt::Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const& args)
	{
		m_suggestIsChosen = true;
	}
	void MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e)
	{
		__super::OnNavigatedTo(e);
	}
}








