package nsu.urdin.chatprotocol.entity;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Builder;
import lombok.Getter;
import lombok.NonNull;

import java.io.Serializable;

@Builder
@Getter
public class Message implements Serializable {
    @NonNull
    private String userName;
    @NonNull
    private String text;

    @JsonCreator
    public Message(@NonNull @JsonProperty("userName") String userName,
                   @NonNull@JsonProperty("text") String text) {
        this.userName = userName;
        this.text = text;
    }
}
