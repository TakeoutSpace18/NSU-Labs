package nsu.urdin.chatprotocol.entity;

import lombok.Builder;
import lombok.Getter;

import java.io.Serializable;

@Builder
@Getter
public class Message implements Serializable {
    private String user;
    private String text;
}
