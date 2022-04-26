clear; clc;
format shorteng;

Fc = 13.56e6;
w = 2 * pi * Fc;

La = 3.65e-6;
Ca = 2.4e-12;
Ra = 1;

L0 = .560e-6;
Rmatch = 50;

Q = 48.47;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

w = (2 * pi * Fc);
Qa = (w * La) / (Ra)
Rq = .5 * ( ( ( w * La)/35) - Ra )

Rd = 848e3;

Lpa = La;
Cpa = Ca;
Rpa = ( (w*La)^2 / (Ra + (2*Rq)));

Fs1 = Fc - Rd;
Fs2 = Fc + Rd;

C0 = 1 / ( (2 * pi * Fs2)^2 * L0);


P1 = 1 - w^2 * L0 * C0;
P2 = P1^2;
P3 = w * (Rmatch/2) * C0;
P4 = P3^2;


Rtr = Rmatch / (P2 + P4);

Xtr = 2 * w * ( (L0 * P1-Rmatch^2/4 * C0 ) / (P2+P4) );

C1 = 1/ (w * (sqrt(((Rtr*Rpa)/4)) + (Xtr / 2)));

C2 = (1/(w^2 * (Lpa/2))) - (1/(w*sqrt(((Rtr*Rpa)/4)))) - (2*Cpa);

Rq
C0
C1
C2

