package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.dto.request.LoginRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;

@Slf4j
public class LoginHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto) {
        if (requestDto instanceof LoginRequest loginRequest) {

            return new SuccessResponse();
        }
        return null;
    }
}
