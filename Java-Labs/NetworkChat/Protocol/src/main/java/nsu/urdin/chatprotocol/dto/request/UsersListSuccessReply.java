package nsu.urdin.chatprotocol.dto.request;

import lombok.Builder;
import lombok.Getter;
import nsu.urdin.chatprotocol.dto.SuccessReply;
import nsu.urdin.chatprotocol.entity.User;

import java.util.List;

@Getter
@Builder
public class UsersListSuccessReply extends SuccessReply {
    private final List<User> users;
}
