package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;

@Slf4j
public abstract class RequestHandler {
    public abstract ResponseBase handle(RequestBase requestDto);
}
