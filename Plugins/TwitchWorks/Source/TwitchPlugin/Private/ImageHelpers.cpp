// Copyright 2017-2022 HowToCompute. All Rights Reserved.


#include "ImageHelpers.h"

#include "TwitchPlugin.h"
#include "Engine/Texture2D.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"
#include "ImageUtils.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Runtime/Core/Public/Core.h"
#include "GetTwitchEmoteSet.h"
#include "Json.h"


void FImageDownloader::GetImage(FString URL, bool _bAllowCache, FString RelativeCachePath)
{
	// Store the relevant inputs to the class so we can access them laver and convert the relative path to its absolute location on the hard drive
	bAllowCache = _bAllowCache;
	FullCachePath = FPaths::Combine(FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()), RelativeCachePath);

	// Is a cached version enabled and are we allowed to access it? If so, pull the image from the cache!
	if (bAllowCache && FPaths::FileExists(FullCachePath))
	{
		// Do all image-related processing on the game thread (as functions like CreateTransient are not thread-safe)
		AsyncTask(ENamedThreads::GameThread, [=] {
			// Attempt to load the file from the user' hard drive and turn it into an image.
			TArray<uint8> ImageData;
			FFileHelper::LoadFileToArray(ImageData, *FullCachePath);
			UTexture2D* Texture = byteArrayToTexture(ImageData);

			// Ensure the image is actually valid
			if (Texture)
			{
				// Attempt to call the callback with the newly created texture
				OnSuccess.ExecuteIfBound(Texture);
			}
			else
			{
				// Attempt to still perform the HTTP request (which will replace the likely corrupted cached image)
				// NOTE: This might start the HTTP request on the game thread, but since it's the request is async itself/runs on a HTTP thread, it shouldn't cause any large-scale blockage.
				StartHttpRequest(URL);
			}
		});

		// Don't execute the rest of the code!
		return;
	}

	// No cache available - download the image from the URL
	StartHttpRequest(URL);
}


void FImageDownloader::StartHttpRequest(FString URL)
{
	// Create and execute an HTTP request to download the image
	FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
	HttpRequest->SetURL(URL);
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FImageDownloader::HandleHTTPResponse);
	HttpRequest->ProcessRequest();
}

void FImageDownloader::HandleHTTPResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// Was the HTTP request successful?
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		// Cache this request if emote caching is enabled.
		if (bAllowCache)
		{
			FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *FullCachePath);
		}

		// (Attempt to) turn the response into an image on the game thread
		// Do all image-related processing on the game thread (as functions like CreateTransient are not thread-safe)
		AsyncTask(ENamedThreads::GameThread, [=]() {
			if (!HttpResponse.IsValid())
			{
				UE_LOG(LogTwitchWorks, Warning, TEXT("Unable to download emoticon: %s"), *HttpResponse->GetContentAsString());
				OnFail.ExecuteIfBound(nullptr);
				return;
			}
			UTexture2D* Texture = byteArrayToTexture(HttpResponse->GetContent());

			if (!Texture)
			{
				OnFail.ExecuteIfBound(nullptr);
				return;
			}

			OnSuccess.ExecuteIfBound(Texture);
			return;
		});

		return;
	}

	// If we hit this something went wrong, pass this on so it calls the OnFail delegate
	OnFail.ExecuteIfBound(nullptr);
	return;
}

UTexture2D* FImageDownloader::byteArrayToTexture(TArray<uint8> ImageData)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrappers[3] =
	{
		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
		ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
	};

	for (auto ImageWrapper : ImageWrappers)
	{
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
		{
			TArray64<uint8>* RawData = new TArray64<uint8>();
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, *RawData))
			{
				if (UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
				{
					Texture->SRGB = true;

					// Attempt to create a mipmap
					uint8* MipData = static_cast<uint8*>(Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

					/*
					*  Start of uncompressed data -> RGB
					*/
					TArray64<uint8>* uncompressedRGBA = new TArray64<uint8>();
					ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, *uncompressedRGBA);

					TArray<FColor> ImageColorData;

					// Turn the raw data into an FColor array
					if (!uncompressedRGBA) { return nullptr; } // Error occurred!
					for (int i = 0; i < uncompressedRGBA->Num(); i++) {
						uint8 tempColorValue;
						FColor tempRGB;

						tempColorValue = (*uncompressedRGBA)[i];
						tempRGB.R = tempColorValue;
						i++;
						tempColorValue = (*uncompressedRGBA)[i];
						tempRGB.G = tempColorValue;
						i++;
						tempColorValue = (*uncompressedRGBA)[i];
						tempRGB.B = tempColorValue;
						i++;
						tempColorValue = (*uncompressedRGBA)[i];
						tempRGB.A = tempColorValue;
						ImageColorData.Add(tempRGB);
					}

					/*
					*  End of uncompressed data -> RGB
					*/

					/*
					*  Start of RGB -> MipMap Data
					*/
					uint8* destPtr = NULL;
					const FColor* srcPtr = NULL;
					bool UseAlpha = true;

					for (int32 y = 0; y < ImageWrapper->GetHeight(); y++)
					{
						destPtr = &MipData[(ImageWrapper->GetHeight() - 1 - y) * ImageWrapper->GetWidth() * sizeof(FColor)];
						srcPtr = const_cast<FColor*>(&ImageColorData[(ImageWrapper->GetHeight() - 1 - y) * ImageWrapper->GetWidth()]);
						for (int32 x = 0; x < ImageWrapper->GetWidth(); x++)
						{
							*destPtr++ = srcPtr->B;
							*destPtr++ = srcPtr->G;
							*destPtr++ = srcPtr->R;
							if (UseAlpha)
							{
								*destPtr++ = srcPtr->A;
							}
							else
							{
								*destPtr++ = 0xFF;
							}
							srcPtr++;
						}
					}

					/*
					*  End of RGB -> MipMap Data
					*/

					// Finalize the mip
					Texture->PlatformData->Mips[0].BulkData.Unlock();
					Texture->UpdateResource();

					return Texture;
				}
			}
		}
	}

	// If we hit this that means something went wrong!
	return nullptr;
}