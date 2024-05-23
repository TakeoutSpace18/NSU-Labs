package nsu.urdin.chatprotocol.dto.request;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;

@Getter
public class RegisterRequest extends RequestBase {
    private final String name;
    private final String password;

    @JsonCreator
    public RegisterRequest(@JsonProperty("name") String name,
                           @JsonProperty("password") String password) {
        super("register");
        this.name = name;
        this.password = password;
    }
}
