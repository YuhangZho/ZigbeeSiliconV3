'''
Created on July 25, 2013

@author: shyang
'''

import math
from .si4010apicalcconstants import constants
from .si4010apicalcutilities import CalcUtils

__all__ = ["Si4010ApiCalcPower"]

class Si4010ApiCalcPower(object):
    '''
    classdocs
    '''
    #
    Vswing_Max      = 3.50001
    Itrim_Target    = 7.6667
    Vswing_Table    = []
    Ptarg           = -1
    pa_lvl_Table    = []
    Rpa             = -1
    Rpant_ref_Table = []
    Iout_Table      = []
    SOIC_Bond_Wire  = -1
    Assumed_Bond_Wire_DC_Res = -1
    Skin_Depth      = -1
    AC_Resistance   = -1
    Diff_Pkg_Parallel_Res  = -1
    Diff_Pkg_Par_Impedance = -1
    Qpar_PA         = -1
    Diff_Pkg_Series_Res = -1
    Diff_Pkg_Series_Impedance = -1
    Diff_PA_Series_Impedance = -1

    Qseries_PA      = -1
    Diff_PA_Par_Res = -1
    Diff_PA_Par_Impedance = -1
    Diff_Parallel_Ant_Res = -1
    Parallel_Combo_with_Rpkg = -1
    Diff_Par_Ant_Impedance   = -1
    Qpar_Ant        = -1
    Diff_Ant_Series_Res = -1
    Diff_Series_Ant_Impedance = -1

    Qseries_Ant     = -1
    Diff_Reflected_Ant_Res = -1
    Calculated_Optimal_Load_at_PA = -1
    Qant            = -1

    Series_Optimal_Load_at_pkg = -1
    Zs_at_pkg       = -1
    Qs              = -1
    Parallel_Rant_at_Pkg_Pins = -1
    Parallel_Zant   = -1
    Attenuation_Factor = -1
    Prad_Table   = []
    DeltaR_Table = []
    Iout_Act_Table = []
    Pout_Table     = []
    Rload_Table    = []
    DrvLvl_Table   = []
    MaxDrv_Table   = []

    Search       = -1
    Result_Itarg        = -1
    Result_Iact         = -1
    Result_DrvLvl       = -1
    Result_MaxDrv       = -1
    Result_Rtarg        = -1

    CR_AT_Real_Z        = -1
    CR_AT_Imag_Z        = -1

    API_vPa_Setup__bMaxDrv     = -1
    API_vPa_Setup__bLevel      = -1

    def __init__ (self, inputs, PA_Object):
        #
        '''
        Constructor
        '''
        self.inputs = inputs
        self.original_inputs = self.inputs

        self.commonInit(PA_Object)

        if self.inputs.AntennaSetup.Manual_Impedance_Entry == True:
            self.calcManualImpedanceEntry()
        else:
            self.calcNormal()

        self.commonPostInit(PA_Object)

        #self.dump() # TODO DEL WHEN RELEASE
        return

    def calcNormal(self):
        #print("Calc Normal Impedance")
        # -------- Ptarg/Rpa is OK -------- 
        # -------- Vswing Table. 40 elements --------
        self.genTable_Vswing()
        # --------  Iout Table -------- 
        self.genTable_Iout()
        #  --------  MaxDrv  -------- 
        self.genTable_MaxDrv()
        # -------- DrvLvl  -------- 
        self.genTable_DrvLvl()
        # -------- pa_lvl  -------- 
        self.pa_lvl_Table = [constants.ROM_PA_DrvLvl[i] for i in self.DrvLvl_Table]
        # -------- Iout_Act  --------
        self.genTable_Iout_Act()
        # -------- Rload  -------- 
        self.genTable_Rload()
        # -------- Rpant_ref --------
        self.genTable_Rpant_ref()
        # -------- DeltaR  -------- 
        self.DeltaR_Table = [abs(x - self.Rpa) for x in self.Rpant_ref_Table]
        # -------- Pout  -------- 
        self.genTable_Pout()
        # -------- Prad  -------- 
        self.genTable_Prad()
        self.calResult()

    def genTable_Vswing (self):
        v = 0.1
        self.Vswing_Table = [v]
        for i in range(1, 40):
            v = (v + 0.1) if (v + 0.1) <= self.Vswing_Max else v
            self.Vswing_Table.append(v)

    def genTable_Rpant_ref(self):
        #IF(USR_IN_Ant_Manual_Impedance_Entry="No",1/(1/$K3-1/$I3),$P38)
        #IF(USR_IN_Ant_Manual_Impedance_Entry="No",1/(1/Rload-1/Rpa),Diff Reflected Ant Res_(Rpant_ref))
        self.Rpant_ref_Table = []
        if self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
            self.Rpant_ref_Table = [1/(1/x - 1/self.Rpa) for x in self.Rload_Table]
        else:
            for m in self.Vswing_Table:
                self.Rpant_ref_Table.append(self.Diff_Reflected_Ant_Res)
        #print("Len of Rpant_ref:", len(self.Rpant_ref_Table))

    def genTable_Rload(self):
        # IF(USR_IN_Ant_Manual_Impedance_Entry="No",1000*Vswing/($G3*2/PI()),1/(1/Rpa+1/Rpant_ref))
        # IF(USR_IN_Ant_Manual_Impedance_Entry="No",1000*Vswing/(Iout_Act*2/PI()),1/(1/Rpa+1/Rpant_ref))
        if self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
            self.Rload_Table = [1000*x/(y*2/math.pi) for (x, y) in zip(self.Vswing_Table, self.Iout_Act_Table)]
        else:
            self.Rload_Table = [1/(1/self.Rpa + 1/m) for m in self.Rpant_ref_Table]
        #print("Len of Rload:", len(self.Rload_Table), self.Rload_Table)

    def genTable_Iout (self):
        '''
        "IF(USR_IN_Ant_Manual_Impedance_Entry=""No"",
            2*PI()/4*2/Vswing*10^((USR_IN_PA_Total_PA_Power-30)/10)*1000,
         ELSE
            IF((2*Rload*10^((Ptarg-30)/10))^0.5<Vswing_max,
                (2*Rload*10^((Ptarg-30)/10))^0.5/Rload*2*PI()/4*1000,
            ELSE
                Vswing_max/Rload*2*PI()/4*1000))"
        '''
        T = []
        for i in range(len(self.Vswing_Table)):
            if self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
                # 2*PI()/4*2/Vswing*10^((USR_IN_PA_Total_PA_Power-30)/10)*1000,
                Iout = 2*math.pi/4*2/self.Vswing_Table[i]*10**((self.inputs.PowerAmplifierSetup.Total_PA_Power - 30)/10)*1000
            else:
                # IF((2*Rload*10^((Ptarg-30)/10))^0.5<Vswing_max,
                Iout = (2*self.Rload_Table[i]*10**((self.Ptarg - 30)/10))**0.5
                if (Iout < self.Vswing_Max):
                    # (2*Rload*10^((Ptarg-30)/10))^0.5/Rload*2*PI()/4*1000,
                    Iout = Iout
                else:
                    #  Vswing_max/Rload*2*PI()/4*1000))"
                    Iout = self.Vswing_Max
                Iout = Iout/self.Rload_Table[i]*2*math.pi/4*1000
            T.append(Iout)
        #print("Len of Iout:", len(T))
        self.Iout_Table = T


            
    def genTable_MaxDrv(self):
        self.MaxDrv_Table = []
        for I in self.Iout_Table :
            tmp = (1 if self.inputs.PowerAmplifierSetup.Center_Frequency < 90 else (1.018 - 0.0002*self.inputs.PowerAmplifierSetup.Center_Frequency))
            md = 1 if (I > self.Itrim_Target*tmp) else 0
            self.MaxDrv_Table.append(md)
        #print("Len of MaxDrv:", len(self.MaxDrv_Table))

    def genTable_DrvLvl(self):
        #ROUND(77+IF(20*LOG(Iout/(Itrim_Target*'Calc Power'!$P$48)/(1+MaxDrv/2))/0.253303>0,0,IF(20*LOG(Iout/(Itrim_Target*'Calc Power'!$P$48)/(1+MaxDrv/2))/0.253303<-77,-77,20*LOG(Iout/(Itrim_Target*'Calc Power'!$P$48)/(1+MaxDrv/2))/0.253303)),0)
        #"ROUND(77+
        #   IF      (20*LOG(Iout/(Itrim_Target*'Calc Power'!Attenuation Factor)/(1+MaxDrv/2))/0.253303>0,
        #       0,
        #   ELSE
        #       IF  (20*LOG(Iout/(Itrim_Target*'Calc Power'!Attenuation Factor)/(1+MaxDrv/2))/0.253303<-77,
        #           -77,
        #       ELSE
        #            20*LOG(Iout/(Itrim_Target*'Calc Power'!Attenuation Factor)/(1+MaxDrv/2))/0.253303))
        #,0)"

        self.DrvLvl_Table = []
        for i in range(len(self.Iout_Table)):
            #20*LOG(Iout/(Itrim_Target*'Calc Power'!Attenuation Factor)/(1+MaxDrv/2))/0.253303))
            dl = self.Iout_Table[i]/(self.Itrim_Target*self.Attenuation_Factor)/(1 + self.MaxDrv_Table[i]/2.0)
            dl = 20*math.log(dl, 10)/0.253303
            if dl > 0:
                dl =  0
            elif dl < -77:
                dl = -77
            dl = int(round(77 + dl + 1e-5, 0)) # In case of 0.5
            self.DrvLvl_Table.append(dl)
        #print("Len of DrvLvl:", len(self.DrvLvl_Table))


    def genTable_Pout(self):
        #IF(USR_IN_Ant_Manual_Impedance_Entry="No",10*LOG((Iout Act*0.001*2/PI())^2*Rload/2)+30,10*LOG((Iout Act/1000/2*4/PI())^2*Rload/2)+30)
        self.Pout_Table = []
        for i in range(len(self.Iout_Act_Table)):
            if self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
                p = 10*math.log((self.Iout_Act_Table[i]*0.001*2 /math.pi)**2*self.Rload_Table[i]/2, 10) + 30
            else:
                p = 10*math.log((self.Iout_Act_Table[i]/1000/2*4/math.pi)**2*self.Rload_Table[i]/2, 10) + 30
            self.Pout_Table.append(p)

    def genTable_Prad(self):
        #"IF(Rpant_ref>0,
        #    10*LOG(10^((Pout-30)/10)*
        #    1/(1/Rload-USR_IN_Ant_Approx_Efficiency/100/Rpant_ref)/
        #  ((1/(1/Rload-USR_IN_Ant_Approx_Efficiency/100/Rpant_ref))
        #   +1/(1/Rpant_ref*USR_IN_Ant_Approx_Efficiency/100)))+30,
        #ELSE
        #    -1000)"
        self.Prad_Table = []
        for i in range(len(self.Rload_Table)):
            if self.Rpant_ref_Table[i] > 0:
                x = 1/(1/self.Rload_Table[i] - self.inputs.AntennaSetup.Approx_Efficiency/100/self.Rpant_ref_Table[i])
                y = x
                z = 1/(1/self.Rpant_ref_Table[i]*self.inputs.AntennaSetup.Approx_Efficiency/100)
                p = 10*math.log(10**((self.Pout_Table[i] - 30)/10) * x/(y + z), 10) + 30
            else:
                p = -1000
            self.Prad_Table.append(p)

    def genTable_Iout_Act(self):
        #FLOOR(pa_lvl/8,1)*(32+MOD(pa_lvl,8))*Itrim_Target*'Calc Power'!Attenuation Factor*(1+MaxDrv/2)/31/39
        self.Iout_Act_Table = []
        for i in range(len(self.pa_lvl_Table)):
            ia = math.floor(self.pa_lvl_Table[i]/8.0 + 1e-5)*(32 + (self.pa_lvl_Table[i]%8))*self.Itrim_Target*self.Attenuation_Factor*(1 + self.MaxDrv_Table[i]/2.0)/31/39
            self.Iout_Act_Table.append(ia)

    def calResult(self):
        # -------- Search --------
        #"IF(AND(USR_IN_PA_PA_Power_Type=""Max Radiated Power"",USR_IN_Ant_Manual_Impedance_Entry=""No""),
        #    MAX(Rload_Table), ; Find the max value in Reload_Table
        #                      ; Search the max in Prad Table and get the corresponding value in xxx
        #ELSE
        #    IF(AND(USR_IN_PA_PA_Power_Type=""Min PA & Max Radiated Power"", USR_IN_Ant_Manual_Impedance_Entry=""No""),
        #        MIN(DeltaR_Table) : MIN($E$3:$E$43)  ; Find the max value in DeltaR_Table
        #                                             ; Get the corresponding value in xxx
        #    ELSE
        #        ,Rload[0] : $D$3))"  ; Use the first value in xxx

        if self.inputs.PowerAmplifierSetup.Drive_Type == "Max Radiated Power" and self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
            self.Search = max(self.Prad_Table)  # TODO why to $D$43
            ind = self.Prad_Table.index(self.Search)
        elif self.inputs.PowerAmplifierSetup.Drive_Type == "Min PA & Max Radiated Power" and self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
            self.Search = min(self.DeltaR_Table)
            ind = self.DeltaR_Table.index(self.Search)
        else:
            self.Search = self.Prad_Table[0]
            ind = 0
        #print("index :", ind)
        self.Result_Itarg  = self.Iout_Table[ind]
        self.Result_Iact   = self.Iout_Act_Table[ind]
        self.Result_DrvLvl = self.DrvLvl_Table[ind]
        self.Result_MaxDrv = self.MaxDrv_Table[ind]
        self.Result_Rtarg  = self.Rload_Table[ind]


    def calcManualImpedanceEntry(self):
        #print("Calc Manual Impedance")
        # -------- Ptarg/Rpa is OK -------- 
        # -------- Vswing Table. 40 elements --------
        self.genTable_Vswing()
        # -------- Rpant_ref --------
        self.genTable_Rpant_ref()
        # -------- Rload  -------- 
        self.genTable_Rload()
        # --------  Iout Table -------- 
        self.genTable_Iout()
        # -------- DeltaR  -------- 
        self.DeltaR_Table = [abs(x - self.Rpa) for x in self.Rpant_ref_Table]
        #  --------  MaxDrv  -------- 
        #IF(Iout>Itrim_Target*(IF(USR_IN_PA_Center_Frequency<90,1,(1.018-0.0002*USR_IN_PA_Center_Frequency))),1,0)
        self.genTable_MaxDrv()
        # -------- DrvLvl  -------- 
        self.genTable_DrvLvl()
        # -------- pa_lvl  -------- 
        self.pa_lvl_Table = [constants.ROM_PA_DrvLvl[i] for i in self.DrvLvl_Table]
        # -------- Iout_Act  --------
        self.genTable_Iout_Act()
        # -------- Pout  -------- 
        self.genTable_Pout()
        # -------- Prad  -------- 
        self.genTable_Prad()

        self.calResult()


    def commonInit(self, PA_Object):
        if self.inputs.General.Package == 1:
            self.SOIC_Bond_Wire = 0.6
            self.Assumed_Bond_Wire_DC_Res = 0.02
        else:
            self.SOIC_Bond_Wire = 1.35
            self.Assumed_Bond_Wire_DC_Res = 0.05
        #SQRT(2*0.0000000244/(2*PI()*USR_IN_PA_Center_Frequency*4*PI()*0.0000001*1000000))*1000000
        self.Skin_Depth = math.sqrt(2*0.0000000244/(2*math.pi*self.inputs.PowerAmplifierSetup.Center_Frequency*4*math.pi*0.0000001*1000000))*1000000
        #0.0000000244/(Skin Depth (um)*0.000001)*0.00104/PI()/(25.4-Skin Depth (um))/0.000001
        self.AC_Resistance = 0.0000000244/(self.Skin_Depth*0.000001)*0.00104/math.pi/(25.4 - self.Skin_Depth)/0.000001
        # CALCULATOR!$G$9  = CR_CI_Real_Z
        self.Diff_Pkg_Parallel_Res  = PA_Object.CR_CI_Real_Z
        # CALCULATOR!$H$9 = CR_CI_Imag_Z
        self.Diff_Pkg_Par_Impedance =  PA_Object.CR_CI_Imag_Z
        #  -1/Diff Pkg Par Impedance (Zp_pkg)*Diff Pkg Parallel Res (Rp_pkg)
        self.Qpar_PA =  -1/self.Diff_Pkg_Par_Impedance*self.Diff_Pkg_Parallel_Res
        #Diff Pkg Parallel Res (Rp_pkg)/(1+Qpar PA^2)
        self.Diff_Pkg_Series_Res = self.Diff_Pkg_Parallel_Res/(1 + self.Qpar_PA**2)
        # 1/(1/Diff Pkg Par Impedance_(Zp_pkg)*(1+Qpar PA^2)/Qpar PA^2)
        self.Diff_Pkg_Series_Impedance = 1/(1/self.Diff_Pkg_Par_Impedance*(1 + self.Qpar_PA**2)/self.Qpar_PA**2)
        #Diff Pkg Series Impedance_(Zs_pkg)-SOIC Bond Wire_(nH)*2*2*PI()*USR_IN_PA_Center_Frequency*1000000*0.000000001
        self.Diff_PA_Series_Impedance = self.Diff_Pkg_Series_Impedance - self.SOIC_Bond_Wire*2*2*math.pi*self.inputs.PowerAmplifierSetup.Center_Frequency*1000000*0.000000001
        #1/Diff PA Series Impedance_(Zs_pa)*Diff Pkg Series Res_(Rs_pkg)
        self.Qseries_PA = 1/self.Diff_PA_Series_Impedance*self.Diff_Pkg_Series_Res
        #(1+Qseries PA^2)/Qseries PA^2*Diff Pkg Series Res_(Rs_pkg)
        self.Diff_PA_Par_Res = (1 + self.Qseries_PA**2)/self.Qseries_PA**2*self.Diff_Pkg_Series_Res
        #1/(1/Diff PA Series Impedance_(Zs_pa)/(1+Qseries PA^2))
        self.Diff_PA_Par_Impedance = 1/(1/self.Diff_PA_Series_Impedance/(1 + self.Qseries_PA**2))
        self.Diff_Parallel_Ant_Res = self.inputs.AntennaSetup.Antenna_Real_Z
        #1/(1/$P31+1/CALCULATOR!$G$9) = 1/(1/$P31+1/CR_CI_Real_Z)
        self.Parallel_Combo_with_Rpkg =  1/(1/self.Diff_Parallel_Ant_Res + 1/PA_Object.CR_CI_Real_Z)
        # CR_CI_Imag_Z = CALCULATOR!$H$9
        self.Diff_Par_Ant_Impedance   = PA_Object.CR_CI_Imag_Z
        #  -1/Diff Par Ant Impedance_(Zp_ant)*Parallel Combo with Rpkg
        self.Qpar_Ant =  -1/self.Diff_Par_Ant_Impedance*self.Parallel_Combo_with_Rpkg
        # Parallel Combo with Rpkg/(1+Qpar Ant^2)
        self.Diff_Ant_Series_Res = self.Parallel_Combo_with_Rpkg/(1 + self.Qpar_Ant**2)
        # 1/(1/Diff Par Ant Impedance_(Zp_ant)*(1+Qpar Ant^2)/Qpar Ant^2)-2*PI()*SOIC Bond Wire_(nH)*2*0.000000001*1000000*USR_IN_PA_Center_Frequency
        self.Diff_Series_Ant_Impedance = 1/(1/self.Diff_Par_Ant_Impedance*(1 + self.Qpar_Ant**2)/self.Qpar_Ant**2)-2*math.pi*self.SOIC_Bond_Wire*2*0.000000001*1000000*self.inputs.PowerAmplifierSetup.Center_Frequency
        #  -1/Diff Series Ant Impedance_(Zs_ant)*Diff Ant Series Res_(Rs_ant)
        self.Qseries_Ant =  -1/self.Diff_Series_Ant_Impedance*self.Diff_Ant_Series_Res
        # 1/(1/((1+Qseries Ant^2)/Qseries Ant^2*Diff Ant Series Res_(Rs_ant))-1/Diff PA Par Res_(Rp_PA))
        self.Diff_Reflected_Ant_Res = 1/(1e-10 + 1/((1 + self.Qseries_Ant**2)/self.Qseries_Ant**2*self.Diff_Ant_Series_Res)-1/self.Diff_PA_Par_Res)
        self.Rpa = self.Diff_PA_Par_Res

        # IF(USR_IN_PA_Center_Frequency<90,1,(1.018-USR_IN_PA_Center_Frequency*0.0002))
        self.Attenuation_Factor = 1 if (self.inputs.PowerAmplifierSetup.Center_Frequency < 90) else (1.018 - self.inputs.PowerAmplifierSetup.Center_Frequency*0.0002)
        
        self.Ptarg = self.inputs.PowerAmplifierSetup.Total_PA_Power
                
                

    def commonPostInit(self, PA_Object):
        # $T$7 = Rtarg
        self.Calculated_Optimal_Load_at_PA = self.Result_Rtarg
        #  -1/Diff PA Par Impedance_(Zp_PA)*Calculated Optimal Load at PA
        self.Qant =  -1/self.Diff_PA_Par_Impedance*self.Calculated_Optimal_Load_at_PA
        # Calculated Optimal Load at PA/(1+Qant^2)
        self.Series_Optimal_Load_at_pkg = self.Calculated_Optimal_Load_at_PA/(1 + self.Qant**2)
        # 1/(1/Diff PA Par Impedance_(Zp_PA)*(1+Qant^2)/Qant^2)+2*SOIC Bond Wire_(nH)*0.000000001*2*PI()*1000000*USR_IN_PA_Center_Frequency
        self.Zs_at_pkg = 1/(1/self.Diff_PA_Par_Impedance*(1+self.Qant**2)/self.Qant**2)+2*self.SOIC_Bond_Wire*0.000000001*2*math.pi*1000000*self.inputs.PowerAmplifierSetup.Center_Frequency
        #  -1/Zs at pkg*Series Optimal Load at pkg
        self.Qs =  -1/self.Zs_at_pkg*self.Series_Optimal_Load_at_pkg
        # Series Optimal Load at pkg*(1+Qs^2)/(Qs^2)
        self.Parallel_Rant_at_Pkg_Pins = self.Series_Optimal_Load_at_pkg*(1 + self.Qs**2)/(self.Qs**2)
        # 1/(1/Zs at pkg/(1+Qs^2))
        self.Parallel_Zant = 1/(1/self.Zs_at_pkg/(1 + self.Qs**2))
        #IF(USR_IN_Ant_Manual_Impedance_Entry="No",-CR_CI_Imag_Z,USR_IN_Ant_Antenna_Image)
        #"IF(USR_IN_Ant_Manual_Impedance_Entry=""No"",
        #    IF (1/(1/'Calc Power'!Parallel Rant at Pkg Pins-1/CR_CI_Real_Z)>0,
        #        1/(1/'Calc Power'!Parallel Rant at Pkg Pins-1/CR_CI_Real_Z),
        #    ELSE
        #        ""Negative Resistance!""),
        #ELSE
        #    USR_IN_Ant_Antenna_Real)"

        if self.inputs.AntennaSetup.Manual_Impedance_Entry == False:
            self.CR_AT_Imag_Z  = -1*PA_Object.CR_CI_Imag_Z
            tmp = 1/(1/self.Parallel_Rant_at_Pkg_Pins - 1/PA_Object.CR_CI_Real_Z)
            self.CR_AT_Real_Z  = tmp if tmp > 0 else "Negative Resistance!"
        else:
            self.CR_AT_Imag_Z  = self.inputs.AntennaSetup.Antenna_Imag_Z
            self.CR_AT_Real_Z  = self.inputs.AntennaSetup.Antenna_Real_Z
        # 'Calc Power'!$S$7    'Calc Power'!Rtarg
        self.API_vPa_Setup__bMaxDrv  = self.Result_MaxDrv
        #'Calc Power'!$R$7   'Calc Power'!MaxDrv
        self.API_vPa_Setup__bLevel   = self.Result_DrvLvl



    def dump(self):
        print("    Vswing_Max                       :",    self.Vswing_Max                             )
        print("    Itrim_Target                     :",    self.Itrim_Target                           )
        print("                                     :",                                                )
        print("    SOIC_Bond_Wire                   :",    self.SOIC_Bond_Wire                         )
        print("    Assumed_Bond_Wire_DC_Res         :",    self.Assumed_Bond_Wire_DC_Res               )
        print("    Skin_Depth                       :",    self.Skin_Depth                             )
        print("    AC_Resistance                    :",    self.AC_Resistance                          )
        print("                                     :",                                                )
        print("    Diff_Pkg_Parallel_Res            :",    self.Diff_Pkg_Parallel_Res                  )
        print("    Diff_Pkg_Par_Impedance           :",    self.Diff_Pkg_Par_Impedance                 )
        print("    Qpar_PA                          :",    self.Qpar_PA                                )
        print("    Diff_Pkg_Series_Res              :",    self.Diff_Pkg_Series_Res                    )
        print("    Diff_Pkg_Series_Impedance        :",    self.Diff_Pkg_Series_Impedance              )
        print("    Diff_PA_Series_Impedance         :",    self.Diff_PA_Series_Impedance               )
        print("    Qseries_PA                       :",    self.Qseries_PA                             )
        print("    Diff_PA_Par_Res                  :",    self.Diff_PA_Par_Res                        )
        print("    Diff_PA_Par_Impedance            :",    self.Diff_PA_Par_Impedance                  )
        print("                                     :",                                                )
        print("    Diff_Parallel_Ant_Res            :",    self.Diff_Parallel_Ant_Res                  )
        print("    Parallel_Combo_with_Rpkg         :",    self.Parallel_Combo_with_Rpkg               )
        print("    Diff_Par_Ant_Impedance           :",    self.Diff_Par_Ant_Impedance                 )
        print("    Qpar_Ant                         :",    self.Qpar_Ant                               )
        print("    Diff_Ant_Series_Res              :",    self.Diff_Ant_Series_Res                    )
        print("    Diff_Series_Ant_Impedance        :",    self.Diff_Series_Ant_Impedance              )
        print("    Qseries_Ant                      :",    self.Qseries_Ant                            )
        print("    Diff_Reflected_Ant_Res           :",    self.Diff_Reflected_Ant_Res                 )
        print("                                     :",                                                )
        print("    Calculated_Optimal_Load_at_PA    :",    self.Calculated_Optimal_Load_at_PA          )
        print("    Qant                             :",    self.Qant                                   )
        print("    Series_Optimal_Load_at_pkg       :",    self.Series_Optimal_Load_at_pkg             )
        print("    Zs_at_pkg                        :",    self.Zs_at_pkg                              )
        print("    Qs                               :",    self.Qs                                     )
        print("    Parallel_Rant_at_Pkg_Pins        :",    self.Parallel_Rant_at_Pkg_Pins              )
        print("    Parallel_Zant                    :",    self.Parallel_Zant                          )
        print("                                     :",                                                )
        print("    Attenuation_Factor               :",    self.Attenuation_Factor                     )
        print("                                     :",                                                )
        print("    Rpa                              :",    self.Rpa                                    )
        print("    Ptarg                            :",    self.Ptarg                                  )
        print("                                     :",                                                )
        print("    Search                           :",    self.Search                                 )
        print("    Itarg                            :",    self.Result_Itarg                           )
        print("    Iact                             :",    self.Result_Iact                            )
        print("    DrvLvl                           :",    self.Result_DrvLvl                          )
        print("    MaxDrv                           :",    self.Result_MaxDrv                          )
        print("    Rtarg                            :",    self.Result_Rtarg                           )
        print("                                     :",                                                )
        print("    ------------ Calculator Result ------------")
        print("    ------------     Antenna Targets ------------")
        print("    CR_AT_Real_Z                     :",    self.CR_AT_Real_Z                           )
        print("    CR_AT_Imag_Z                     :",    self.CR_AT_Imag_Z                           )
        print("    ------------ API Parameters ------------")
        print("    API:vPa_Setup        :")
        print("            API_vPa_Setup__bMaxDrv:    {}".format(self.API_vPa_Setup__bMaxDrv))
        print("            API_vPa_Setup__bLevel:     {}".format(self.API_vPa_Setup__bLevel))

