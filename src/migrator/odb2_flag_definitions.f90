module odb2_flag_definitions
! Module used in module odb2.F90
! 25-09-2012 Hans Hersbach ECMWF

  implicit none

contains  ! naming and shaming of odb_api bitfield definitions and usage

subroutine odb_find_ctype(cname,ctype)
  character(len=*), intent( in) :: cname
  character(len=*), intent(out) :: ctype

  integer :: k

  k=index(cname,"@") -1 ; if (k==-1) k=len_trim(cname)

!-General rule
  ctype=cname(:k)//"_t"

!-Exceptions
  if(cname(:k)== "datum_status") ctype="status_t"
  if(cname(:k)== "datum_anflag") ctype="datum_flag_t"
  if(cname(:k)=="report_status") ctype="status_t"

end subroutine odb_find_ctype

subroutine get_odb_flags(ctype,bdef,bits)
! This should be a straight translation from type_definitions.h
! Ensure that everything is LOWERCASE

  character(len=*), intent( in) :: ctype
  character(len=*), intent(out) :: bdef,bits

  character(len=1024) :: cbits

  cbits=""
  if (trim(ctype)=="report_rdbflag_t") then
     cbits=""                &
     &// "lat_humon     :1;" &
     &// "lat_qcsub     :1;" &
     &// "lat_override  :1;" &
     &// "lat_flag      :2;" &
     &// "lat_hqc_flag  :1;" &
     &// "lon_humon     :1;" &
     &// "lon_qcsub     :1;" &
     &// "lon_override  :1;" &
     &// "lon_flag      :2;" &
     &// "lon_hqc_flag  :1;" &
     &// "date_humon    :1;" &
     &// "date_qcsub    :1;" &
     &// "date_override :1;" &
     &// "date_flag     :2;" &
     &// "date_hqc_flag :1;" &
     &// "time_humon    :1;" &
     &// "time_qcsub    :1;" &
     &// "time_override :1;" &
     &// "time_flag     :2;" &
     &// "time_hqc_flag :1;" &
     &// "stalt_humon   :1;" &
     &// "stalt_qcsub   :1;" &
     &// "stalt_override:1;" &
     &// "stalt_flag    :2;" &
     &// "stalt_hqc_flag:1;" &
     &// ""
  endif

  if (trim(ctype)=="status_t") then
     cbits=""                 &
     &// "active         :1;" &        !// ACTIVE FLAG
     &// "passive        :1;" &        !// PASSIVE FLAG
     &// "rejected       :1;" &        !// REJECTED FLAG
     &// "blacklisted    :1;" &        !// BLACKLISTED
     &// "use_emiskf_only:1;" &        !// to be used for emiskf only
     &// ""
  endif

  if (trim(ctype)=="datum_rdbflag_t") then
     cbits=""                      &
     &// "press_humon         :1;" &
     &// "press_qcsub         :1;" &
     &// "press_override      :1;" &
     &// "press_flag          :2;" &
     &// "press_hqc_flag      :1;" &
     &// "press_judged_prev_an:2;" &
     &// "press_used_prev_an  :1;" &
     &// "_press_unused_6     :6;" &
     &// "varno_humon         :1;" &
     &// "varno_qcsub         :1;" &
     &// "varno_override      :1;" &
     &// "varno_flag          :2;" &
     &// "varno_hqc_flag      :1;" &
     &// "varno_judged_prev_an:2;" &
     &// "varno_used_prev_an  :1;" &
!    &// "_varno_unused_6     :6;" &
     &// ""
  endif

  if (trim(ctype)=="datum_flag_t") then
     cbits=""           &
     &// "final    :4;" &                    !  // FINAL FLAG
     &// "fg       :4;" &                    !  // FIRST GUESS FLAG
     &// "depar    :4;" &                    !  // DEPARTURE FLAG
     &// "varqc    :4;" &                    !  // VARIATIONAL QUALITY FLAG
     &// "blacklist:4;" &                    !  // BLACKLIST FLAG
     &// "ups      :1;" &                    !  // d'utilisation par analyse de pression de surface
     &// "uvt      :1;" &                    !  // d'utilisation par analyse de vent et temperature
     &// "uhu      :1;" &                    !  // d'utilisation par analyse d'humidite
     &// "ut2      :1;" &                    !  // d'utilisation par analyse de temperat ure a 2m
     &// "uh2      :1;" &                    !  // d'utilisation par analyse d'humidite a 2m
     &// "uv1      :1;" &                    !  // d'utilisation par analyse de vent a 10m
     &// "urr      :1;" &                    !  // d'utilisation par analyse de precipitations
     &// "usn      :1;" &                    !  // d'utilisation par analyse de neige
     &// "usst     :1;" &                    !  // d'utilisation par analyse de temperature de surface de la mer
     &// ""
  endif

  if (trim(ctype)=="level_t") then
     cbits=""             &
!    &// "id         :9;" &                  !  // PILOT LEV. ID.
     &// "maxwind    :1;" &                  !  // MAX WIND LEVEL
     &// "tropopause :1;" &                  !  // TROPOPAUSE
     &// "d_part     :1;" &                  !  // D PART
     &// "c_part     :1;" &                  !  // C PART
     &// "b_part     :1;" &                  !  // B PART
     &// "a_part     :1;" &                  !  // A PART
     &// "surface    :1;" &                  !  // SURFACE LEVEL
     &// "signwind   :1;" &                  !  // SIGNIFICANT WIND LEVEL
     &// "signtemp   :1;" &                  !  // SIGNIFICANT TEMPR. LEVEL
     &// ""
  endif

  if (trim(ctype)=="report_event1_t") then
     cbits=""               &
     &// "no_data      :1;" &                !  // no data in the report
     &// "all_rejected :1;" &                !  // all data rejected
     &// "bad_practice :1;" &                !  // bad reporting practice
     &// "rdb_rejected :1;" &                !  // rejected due to RDB flag
     &// "redundant    :1;" &                !  // redundant report
     &// "stalt_missing:1;" &                !  // missing station altitude
     &// "qc_failed    :1;" &                !  // failed quality control
     &// "overcast_ir  :1;" &                !  // report overcast IR
     &// ""
  endif

  if (trim(ctype)=="report_blacklist_t") then
     cbits=""                    &
     &// "obstype           :1;" &
     &// "statid            :1;" &
     &// "codetype          :1;" &
     &// "instype           :1;" &
     &// "date              :1;" &
     &// "time              :1;" &
     &// "lat               :1;" &
     &// "lon               :1;" &
     &// "stalt             :1;" &
     &// "scanpos           :1;" &
     &// "retrtype          :1;" &
     &// "qi_fc             :1;" &
     &// "rff               :1;" &
     &// "qi_nofc           :1;" &
     &// "modoro            :1;" &
     &// "lsmask            :1;" &
     &// "rlsmask           :1;" &
     &// "modps             :1;" &
     &// "modts             :1;" &
     &// "modt2m            :1;" &
     &// "modtop            :1;" &
     &// "sensor            :1;" &
     &// "fov               :1;" &
     &// "satza             :1;" &
     &// "andate            :1;" &
     &// "antime            :1;" &
     &// "solar_elevation   :1;" &
     &// "quality_retrieval :1;" &
     &// "cloud_cover       :1;" &
     &// "cloud_top_pressure:1;" &
     &// "product_type      :1;" &
     &// "sonde_type        :1;" &
     &// ""
  endif

  if (trim(ctype)=="datum_event1_t")then
     cbits=""                     &
     &// "vertco_missing     :1;" &       ! // missing vertical coordinate
     &// "obsvalue_missing   :1;" &       ! // missing observed value
     &// "fg_missing         :1;" &       ! // missing first guess value
     &// "rdb_rejected       :1;" &       ! // rejected due ti RDB flag
     &// "assim_cld_flag     :1;" &       ! // assim of cloud-affected radiance
     &// "bad_practice       :1;" &       ! // bad reporting practice
     &// "vertpos_outrange   :1;" &       ! // vertical position out of range
     &// "fg2big             :1;" &       ! // too big first guess departure
     &// "depar2big          :1;" &       ! // too big departure in assimilation
     &// "obs_error2big      :1;" &       ! // too big observation error
     &// "datum_redundant    :1;" &       ! // dedundant datum
     &// "level_redundant    :1;" &       ! // redundant level
     &// "not_analysis_varno :1;" &       ! // not an analysis variable
     &// "duplicate          :1;" &       ! // duplicated datum/level
     &// "levels2many        :1;" &       ! // too many surface data/levels
     &// "level_selection    :1;" &       ! // level selection
     &// "vertco_consistency :1;" &       ! // vertical consistency check
     &// "vertco_type_changed:1;" &       ! // vertical coordinate changed from Z to P
     &// "combined_flagging  :1;" &       ! // combined flagging
     &// "report_rejected    :1;" &       ! // datum rejected due to rejected report
     &// "varqc_performed    :1;" &       ! // variational QC performed
     &// "obserror_increased :1;" &       ! // obs error increased
     &// "contam_cld_flag    :1;" &       ! // cloud contamination
     &// "contam_rain_flag   :1;" &       ! // rain contamination
     &// "contam_aerosol_flag:1;" &       ! // aerosol contamination
     &// ""
  endif


  if (trim(ctype)=="datum_blacklist_t")then
     cbits=""              &
     &// "varno       :1;" &
     &// "vertco_type :1;" &
     &// "press       :1;" &
     &// "press_rl    :1;" &
     &// "ppcode      :1;" &
     &// "obsvalue    :1;" &
     &// "fg_depar    :1;" &
     &// "obs_error   :1;" &
     &// "fg_error    :1;" &
     &// "winchan_dep :1;" &
     &// "obs_t       :1;" &
     &// "elevation   :1;" &
     &// "winchan_dep2:1;" &
     &// "tausfc      :1;" &
     &// "csr_pclear  :1;" &
     &// ""
  endif

  if (trim(ctype)=="aeolus_hdrflag_t")then
     cbits=""                 &
     &// "nadir_location :1;" &  ! //  bit#0 = 1 for location at nadir (calibration mode)
     &// "orbit_predicted:1;" &  ! //      1         location from orbit predictor
     &// "omit_from_ee   :1;" &  ! //      2         predicted location to omit from EE product
     &// ""
  endif

  if(cbits=="") write(*,'(2a)')"Unknown bitfield: ",trim(ctype)
  call split_odb_flag(cbits,bdef,bits)

end subroutine get_odb_flags

subroutine split_odb_flag(cbits,bdef,bits)
  character(len=*), intent( in) :: cbits
  character(len=*), intent(out) :: bdef,bits

  integer :: i,k,l
  bdef=""; bits=""

  i=0
  do
    k=index(cbits(i+1:),":") ; if (k==0) exit
    l=index(cbits(i+1:),";")
    bdef=trim(bdef)//trim(adjustl(cbits(i  +1:i+k-1)))//":"
    bits=trim(bits)//trim(adjustl(cbits(i+k+1:i+l-1)))//":"
    i=i+l
  enddo

end subroutine split_odb_flag

end module odb2_flag_definitions
