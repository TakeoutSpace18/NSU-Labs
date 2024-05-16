package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.request.UsersListRequest;
import nsu.urdin.chatprotocol.dto.request.UsersListSuccessResponse;
import nsu.urdin.chatprotocol.entity.User;

@Slf4j
public class GetUsersListHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto) {
        if (requestDto instanceof UsersListRequest usersListRequest) {
            return UsersListSuccessResponse.builder()
                    .user(new User("user1"))
                    .user(new User("user2"))
                    .build();
        }
        return null;
    }
}
