package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;

public abstract class RequestHandler {
    public abstract ResponseBase handle(RequestBase requestDto);
}
