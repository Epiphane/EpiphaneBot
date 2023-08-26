
#include "TwitchPubSubConnection.h"
#include "TwitchRuntimeSettings.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "WebSocketsModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogTwitchEventSubsystem, Log, All);

void UTwitchEventsSubsystem::OpenConnection(FTwitchLoginCredentials credentials)
{
    Credentials = credentials;

    const FString ServerURL = TEXT("wss://eventsub.wss.twitch.tv/ws");
    const FString ServerProtocol = TEXT("ws");

    Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
    Socket->OnConnected().AddUObject(this, &UTwitchEventsSubsystem::OnSocketConnected);
    Socket->OnConnectionError().AddUObject(this, &UTwitchEventsSubsystem::OnConnectionError);
    Socket->OnClosed().AddUObject(this, &UTwitchEventsSubsystem::OnClosed);
    Socket->OnMessage().AddUObject(this, &UTwitchEventsSubsystem::OnMessage);
    Socket->OnRawMessage().AddUObject(this, &UTwitchEventsSubsystem::OnRawMessage);
    Socket->OnMessageSent().AddUObject(this, &UTwitchEventsSubsystem::OnMessageSent);

    // And we finally connect to the server. 
    Socket->Connect();
}

void UTwitchEventsSubsystem::Deinitialize()
{
    if (Socket.IsValid())
    {
        Socket->Close();
        Socket = nullptr;
    }

    SessionID.Empty();
}

void UTwitchEventsSubsystem::OnSocketConnected()
{
    UE_LOG(LogTwitchEventSubsystem, Log, TEXT("Connected!"));
}

void UTwitchEventsSubsystem::OnConnectionError(const FString& Error)
{
    UE_LOG(LogTwitchEventSubsystem, Error, TEXT("Connection Error: %s"), *Error);
}

void UTwitchEventsSubsystem::SubscribeToEvent(FString Type, FString Version)
{
    // Create an HTTP request for the user lookup
    FHttpRequestPtr HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Client-ID"), GetMutableDefault<UTwitchRuntimeSettings>()->clientID);
    HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Credentials.OAuthToken));
    HttpRequest->SetHeader(TEXT("X-Request-Type"), Type);
    HttpRequest->SetURL(TEXT("https://api.twitch.tv/helix/eventsub/subscriptions"));

    FEventSubscriptionRequest Request;
    Request.type = Type;
    Request.version = Version;
    Request.condition.broadcaster_user_id = Credentials.UserId;
    Request.condition.moderator_user_id = Credentials.UserId;
    Request.transport.session_id = SessionID;

    FString RequestStr;
    if (!FJsonObjectConverter::UStructToJsonObjectString(Request, RequestStr))
    {
        return;
    }

    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetContentAsString(RequestStr);

    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTwitchEventsSubsystem::HandleHTTPRequest);
    HttpRequest->ProcessRequest();
}

void UTwitchEventsSubsystem::SubscribeToEvents()
{
    SubscribeToEvent(TEXT("channel.follow"), TEXT("2"));
    SubscribeToEvent(TEXT("channel.subscribe"), TEXT("1"));
    SubscribeToEvent(TEXT("channel.channel_points_custom_reward_redemption.add"), TEXT("1"));
    SubscribeToEvent(TEXT("channel.poll.begin"), TEXT("1"));
    SubscribeToEvent(TEXT("channel.poll.progress"), TEXT("1"));
    SubscribeToEvent(TEXT("channel.poll.end"), TEXT("1"));
}

void UTwitchEventsSubsystem::HandleHTTPRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
    FString RequestType = HttpRequest->GetHeader(TEXT("X-Request-Type"));
    UE_LOG(LogTwitchEventSubsystem, Log, TEXT("%s %s %d %s"), *HttpRequest->GetVerb(), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *RequestType);
}

void UTwitchEventsSubsystem::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
    UE_LOG(LogTwitchEventSubsystem, Log, TEXT("Connection Closed"));
    OnDisconnected.Broadcast();
}

void UTwitchEventsSubsystem::OnMessage(const FString& Message)
{
    UE_LOG(LogTwitchEventSubsystem, Log, TEXT("Received message: %s"), *Message);

    TSharedPtr<FJsonObject> EventObj;
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Message);
    if (!FJsonSerializer::Deserialize(JsonReader, EventObj) || !EventObj.IsValid())
    {
        UE_LOG(LogTwitchEventSubsystem, Error, TEXT("Failed to parse websocket message"));
        return;
    }

    TSharedPtr<FJsonObject> MetadataObj = EventObj->GetObjectField("metadata");
    TSharedPtr<FJsonObject> PayloadObj = EventObj->GetObjectField("payload");
    FTwitchEventMetadata Metadata;
    if (!FJsonObjectConverter::JsonObjectToUStruct(MetadataObj.ToSharedRef(), &Metadata))
    {
        UE_LOG(LogTwitchEventSubsystem, Error, TEXT("Unable to deserialize metadata object"));
        return;
    }

    if (Metadata.message_type == ETwitchEventMessageType::session_welcome)
    {
        FTwitchEventSessionPayload Session;
        if (!FJsonObjectConverter::JsonObjectToUStruct(PayloadObj.ToSharedRef(), &Session))
        {
            UE_LOG(LogTwitchEventSubsystem, Error, TEXT("Unable to deserialize session payload"));
            return;
        }

        SessionID = Session.session.id;
        SubscribeToEvents();
        OnConnected.Broadcast(Credentials.Username);
    }
    else if (Metadata.message_type == ETwitchEventMessageType::session_keepalive)
    {
        // TODO
    }
    else if (Metadata.message_type == ETwitchEventMessageType::session_reconnect)
    {
        // TODO
    }
    else if (Metadata.message_type == ETwitchEventMessageType::notification)
    {
        OnNotification(Metadata.subscription_type, PayloadObj->GetObjectField("event"));
    }

}

void UTwitchEventsSubsystem::OnNotification(FString Type, TSharedPtr<FJsonObject> Payload)
{
    if (Type == "channel.channel_points_custom_reward_redemption.add")
    {
        FRedemptionNotification Redemption;
        if (!FJsonObjectConverter::JsonObjectToUStruct(Payload.ToSharedRef(), &Redemption))
        {
            UE_LOG(LogTwitchEventSubsystem, Error, TEXT("Unable to deserialize payload object"));
            return;
        }

        OnChannelPointRedemption.Broadcast(Redemption.reward, Redemption.user_id, Redemption.user_name);
    }
}

void UTwitchEventsSubsystem::OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining)
{
}

void UTwitchEventsSubsystem::OnMessageSent(const FString& MessageString)
{
    UE_LOG(LogTwitchEventSubsystem, Log, TEXT("UTwitchEventsSubsystem::OnMessageSent"));
}
