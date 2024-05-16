package nsu.urdin.chatserver.request;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ErrorResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatserver.request.handlers.GetChatHistoryHandler;
import nsu.urdin.chatserver.request.handlers.GetUsersListHandler;
import nsu.urdin.chatserver.request.handlers.LoginHandler;
import nsu.urdin.chatserver.request.handlers.RequestHandler;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class RequestHandlerChain
{
    private final List<RequestHandler> handlers;

    public RequestHandlerChain() {
        handlers = new ArrayList<>();
        handlers.add(new LoginHandler());
        handlers.add(new GetUsersListHandler());
        handlers.add(new GetChatHistoryHandler());
    }

    public ResponseBase handle(RequestBase requestDto) {
        for (RequestHandler handler : handlers) {
            ResponseBase reply = handler.handle(requestDto);
            if (reply != null) {
                return reply;
            }
        }

        log.error("Unknown request: {}", requestDto);
        return new ErrorResponse("Unknown request");
    }
}
