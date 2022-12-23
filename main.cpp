#include <winapifamily.h>
#include <Windows.h>
#include <chrono>
#include <GdiPlus.h>
#include <stdio.h>
#include <Windows.h>
#include <Gdiplus.h>
#include <gdiplusinit.h>
#include <filesystem>



#include <wholeinclude.h>
#include <helper_functions.h>
#pragma comment(lib,"gdiplus.lib")

using namespace Gdiplus;

HRESULT GetEncoderClsid(const std::wstring& format, GUID* pGuid)
{
	HRESULT hr = S_OK;
	UINT  nEncoders = 0;          // number of image encoders
	UINT  nSize = 0;              // size of the image encoder array in bytes
	std::vector<BYTE> spData;
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	Gdiplus::Status status;
	bool found = false;

	if (format.empty() || !pGuid)
	{
		hr = E_INVALIDARG;
	}

	if (SUCCEEDED(hr))
	{
		*pGuid = GUID_NULL;
		status = Gdiplus::GetImageEncodersSize(&nEncoders, &nSize);

		if ((status != Gdiplus::Ok) || (nSize == 0))
		{
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
	{

		spData.resize(nSize);
		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)&spData.front();
		status = Gdiplus::GetImageEncoders(nEncoders, nSize, pImageCodecInfo);

		if (status != Gdiplus::Ok)
		{
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
	{
		for (UINT j = 0; j < nEncoders && !found; j++)
		{
			if (pImageCodecInfo[j].MimeType == format)
			{
				*pGuid = pImageCodecInfo[j].Clsid;
				found = true;
			}
		}

		hr = found ? S_OK : E_FAIL;
	}

	return hr;
}



int main()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileNameW(NULL, szExeFileName, MAX_PATH);
	std::string exename = std::filesystem::path(szExeFileName).filename().string();
	std::wstring exename_wstring = std::wstring(exename.begin(), exename.end());

	//
	if (killProcessByName((wchar_t*)exename_wstring.c_str()))
	{
		createWindowsMessage("Programm was running. All Instances will be closed.");
		exit;
		abort;
		return 1;
	}

	int interation_name = 0;
	while (std::filesystem::exists(findDocumentFolderAsString() + "\\screenshots\\" + std::to_string(interation_name) + ".jpg"))
		interation_name++;



	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	std::chrono::time_point start_point = std::chrono::system_clock::now();


	while (true)
	{
		std::chrono::time_point now_point = std::chrono::system_clock::now();
		std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(start_point - now_point);

		while (time_span.count() < 4)
		{
			now_point = std::chrono::system_clock::now();
			time_span = duration_cast<std::chrono::duration<double>>(now_point - start_point);
			continue;
		}
		{
			// Get the desktop window
			HWND desktop = GetDesktopWindow();

			// Get the size of the desktop window
			RECT desktopRect;
			GetWindowRect(desktop, &desktopRect);
			int width = desktopRect.right - desktopRect.left;
			int height = desktopRect.bottom - desktopRect.top;

			// Create a device context for the desktop window
			HDC dc = GetDC(desktop);

			// Create a bitmap to store the screenshot
			HBITMAP bitmap = CreateCompatibleBitmap(dc, width, height);

			// Create a device context for the bitmap
			HDC bitmapDC = CreateCompatibleDC(dc);
			SelectObject(bitmapDC, bitmap);

			// Copy the desktop window to the bitmap
			BitBlt(bitmapDC, 0, 0, width, height, dc, 0, 0, SRCCOPY);

			// Clean up
			DeleteDC(bitmapDC);
			ReleaseDC(desktop, dc);

			// Create an image object for the bitmap
			Gdiplus::Bitmap image(bitmap, NULL);

			// Save the image to a file
			CLSID bmpClsid;
			
			GetEncoderClsid(L"image/jpeg", &bmpClsid);
			std::string x = findDocumentFolderAsString() + "\\screenshots\\" + std::to_string(interation_name) + ".jpg";
			std::wstring wide_string = std::wstring(x.begin(), x.end());
			image.Save(wide_string.c_str(), &bmpClsid, NULL);
			interation_name++;
			//Clean up
			try {
				DeleteObject(bitmap);
			}
			catch (std::error_code e)
			{
			}
			// Clean up GDI+
			//try {
			//	Gdiplus::GdiplusShutdown(gdiplusToken);
			//}
			//catch (std::error_code e)
			//{
			//}




		}
		start_point = std::chrono::system_clock::now();
	}
	return 0;

}