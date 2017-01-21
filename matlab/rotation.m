function R = rotation(angles)

    Phi = angles(1);
    The = angles(2);
    Psi = angles(3);

    R = zeros(3);
    R(:, 1) = [
        cos(Psi) * cos(The)
        sin(Psi) * cos(The)
        -sin(The)
    ];
    R(:, 2) = [
        cos(Psi) * sin(The) * sin(Phi) - sin(Psi) * cos(Phi)
        sin(Psi) * sin(The) * sin(Phi) + cos(Psi) * cos(Phi)
        cos(The) * sin(Phi) 
    ];
    R(:, 3) = [
        cos(Psi) * sin(The) * cos(Phi) + sin(Psi) * sin(Phi)
        sin(Psi) * sin(The) * cos(Phi) - cos(Psi) * sin(Phi)
        cos(The) * cos(Phi)
    ];

end
