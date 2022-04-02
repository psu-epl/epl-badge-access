package controllers

import (
	"encoding/json"
	"fmt"
	"net/http"
	"strconv"

	"github.com/gorilla/mux"
	"github.com/psu-epl/epl-badge-access/software/server/labpass-server/lib/db/models"
	"github.com/psu-epl/epl-badge-access/software/server/labpass-server/lib/responses"
)

func (s *Server) CreateLabStation(w http.ResponseWriter, r *http.Request) {

	labStation := models.LabStation{}

	if err := json.NewDecoder(r.Body).Decode(&labStation); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	labStation.Prepare()
	if err := labStation.Validate("create"); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	labStationCreated, err := labStation.Save(s.db)

	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}

	w.Header().Set("Location", fmt.Sprintf("%s%s/%d", r.Host, r.RequestURI, labStationCreated.ID))
	responses.JSON(w, http.StatusCreated, labStationCreated)
}

func (s *Server) GetLabStations(w http.ResponseWriter, _ *http.Request) {

	labStation := models.LabStation{}

	labStations, err := labStation.FindAll(s.db)
	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}
	responses.JSON(w, http.StatusOK, labStations)
}

func (s *Server) GetLabStation(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	uid, err := strconv.ParseUint(vars["id"], 10, 32)
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}
	labStation := models.LabStation{}
	labStationGotten, err := labStation.FindByID(s.db, uint32(uid))
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}
	responses.JSON(w, http.StatusOK, labStationGotten)
}

func (s *Server) UpdateLabStation(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	uid, err := strconv.ParseUint(vars["id"], 10, 32)
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}

	labStation := models.LabStation{}
	if err := json.NewDecoder(r.Body).Decode(&labStation); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	//tokenID, err := auth.ExtractTokenID(r)
	//if err != nil {
	//	responses.ERROR(w, http.StatusUnauthorized, errors.New("Unauthorized"))
	//	return
	//}
	//if tokenID != uint32(uid) {
	//	responses.ERROR(w, http.StatusUnauthorized, errors.New(http.StatusText(http.StatusUnauthorized)))
	//	return
	//}

	labStation.Prepare()
	if err := labStation.Validate("update"); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	updatedLabStation, err := labStation.Update(s.db, uint32(uid))
	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}
	responses.JSON(w, http.StatusOK, updatedLabStation)
}

func (s *Server) DeleteLabStation(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)

	labStation := models.LabStation{}

	uid, err := strconv.ParseUint(vars["id"], 10, 32)
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}
	//tokenID, err := auth.ExtractTokenID(r)
	//if err != nil {
	//	responses.ERROR(w, http.StatusUnauthorized, errors.New("Unauthorized"))
	//	return
	//}
	//if tokenID != 0 && tokenID != uint32(uid) {
	//	responses.ERROR(w, http.StatusUnauthorized, errors.New(http.StatusText(http.StatusUnauthorized)))
	//	return
	//}

	_, err = labStation.Delete(s.db, uint32(uid))
	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}
	w.Header().Set("Entity", fmt.Sprintf("%d", uid))
	responses.JSON(w, http.StatusNoContent, "")
}
