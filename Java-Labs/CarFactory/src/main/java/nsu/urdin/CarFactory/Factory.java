package nsu.urdin.CarFactory;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class Factory {
    @Autowired
    CarFactoryConfig config;
    public Factory() {
    }
}
