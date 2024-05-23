package nsu.urdin.chatprotocol.dto.request;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;

@Getter
public class LoginRequest extends RequestBase {
    private final String name;
    private final String password;

    @JsonCreator
    public LoginRequest(@JsonProperty("name") String name,
                        @JsonProperty("password") String password) {
        super("login");
        this.name = name;
        this.password = password;
    }
}
