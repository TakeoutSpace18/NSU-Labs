package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.request.UsersListRequest;
import nsu.urdin.chatprotocol.dto.request.UsersListSuccessResponse;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.ConnectionSession;

@Slf4j
public class GetUsersListHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof UsersListRequest usersListRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

            return UsersListSuccessResponse.builder()
                    .user(new User("user1"))
                    .user(new User("user2"))
                    .build();
        }
        return null;
    }
}
