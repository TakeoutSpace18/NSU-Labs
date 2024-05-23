package nsu.urdin.chatprotocol.entity;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NonNull;

import java.io.Serializable;

@Builder
@Getter
@AllArgsConstructor
public class Message implements Serializable {
    @NonNull
    private String userName;
    @NonNull
    private String text;
}
