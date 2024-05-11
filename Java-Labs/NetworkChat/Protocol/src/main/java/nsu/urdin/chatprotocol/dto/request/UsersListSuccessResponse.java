package nsu.urdin.chatprotocol.dto.request;

import lombok.Builder;
import lombok.Getter;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.entity.User;

import java.util.List;

@Getter
@Builder
public class UsersListSuccessResponse extends SuccessResponse {
    private final List<User> users;
}
