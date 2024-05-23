package nsu.urdin.chatprotocol.dto.response;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Builder;
import lombok.Getter;
import lombok.Singular;
import nsu.urdin.chatprotocol.entity.User;

import java.util.List;

@Getter
@Builder
public class UsersListSuccessResponse extends SuccessResponse {
    @Singular
    private final List<User> users;

    @JsonCreator
    public UsersListSuccessResponse(@JsonProperty("users") List<User> users) {
        this.users = users;
    }
}
