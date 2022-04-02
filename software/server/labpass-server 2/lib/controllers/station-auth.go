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

func (s *Server) CreateStationAuth(w http.ResponseWriter, r *http.Request) {

	stationAuth := models.StationAuth{}

	if err := json.NewDecoder(r.Body).Decode(&stationAuth); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	stationAuth.Prepare()
	if err := stationAuth.Validate("create"); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	stationAuthCreated, err := stationAuth.Save(s.db)

	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}

	w.Header().Set("Location", fmt.Sprintf("%s%s/%d", r.Host, r.RequestURI, stationAuthCreated.ID))
	responses.JSON(w, http.StatusCreated, stationAuthCreated)
}

func (s *Server) GetStationAuths(w http.ResponseWriter, _ *http.Request) {

	stationAuth := models.StationAuth{}

	stationAuths, err := stationAuth.FindAll(s.db)
	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}
	responses.JSON(w, http.StatusOK, stationAuths)
}

func (s *Server) GetStationAuth(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	uid, err := strconv.ParseUint(vars["id"], 10, 32)
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}
	stationAuth := models.StationAuth{}
	stationAuthGotten, err := stationAuth.FindByID(s.db, uint32(uid))
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}
	responses.JSON(w, http.StatusOK, stationAuthGotten)
}

func (s *Server) UpdateStationAuth(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)
	uid, err := strconv.ParseUint(vars["id"], 10, 32)
	if err != nil {
		responses.ERROR(w, http.StatusBadRequest, err)
		return
	}

	stationAuth := models.StationAuth{}
	if err := json.NewDecoder(r.Body).Decode(&stationAuth); err != nil {
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

	stationAuth.Prepare()
	if err := stationAuth.Validate("update"); err != nil {
		responses.ERROR(w, http.StatusUnprocessableEntity, err)
		return
	}

	updatedStationAuth, err := stationAuth.Update(s.db, uint32(uid))
	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}
	responses.JSON(w, http.StatusOK, updatedStationAuth)
}

func (s *Server) DeleteStationAuth(w http.ResponseWriter, r *http.Request) {

	vars := mux.Vars(r)

	stationAuth := models.StationAuth{}

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

	_, err = stationAuth.Delete(s.db, uint32(uid))
	if err != nil {
		responses.ERROR(w, http.StatusInternalServerError, err)
		return
	}
	w.Header().Set("Entity", fmt.Sprintf("%d", uid))
	responses.JSON(w, http.StatusNoContent, "")
}
