package models

import (
	"errors"
	"fmt"
	"time"

	"gorm.io/gorm"
)

type User struct {
	ID        uint `gorm:"primarykey" json:"id"`
	CreatedAt time.Time
	UpdatedAt time.Time
	DeletedAt gorm.DeletedAt `gorm:"index"`
	LastName  string         `json:"lastName"`
	FirstName string         `json:"firstName"`
	BadgeID   string         `gorm:"index" json:"badgeId"`
	Admin     bool           `json:"admin"`
}

func (u *User) Prepare() {
}

func (u *User) Validate(action string) error {
	return nil
}

func (u *User) Save(db *gorm.DB) (*User, error) {
	var err error
	err = db.Debug().Create(&u).Error
	if err != nil {
		return &User{}, err
	}
	return u, nil
}

func (u *User) FindAll(db *gorm.DB, query map[string][]string) (*[]User, error) {
	var err error
	users := []User{}
	otherUsers := []User{}

	err = db.Debug().Model(&User{}).Where(query["filter"]).Limit(100).Find(&otherUsers).Error
	if err != nil {
		fmt.Printf("%v\n", err)
	}

	err = db.Debug().Model(&User{}).Limit(100).Find(&users).Error
	if err != nil {
		return &[]User{}, err
	}
	return &users, err

}

func (u *User) FindByID(db *gorm.DB, id uint32) (*User, error) {
	var err error
	err = db.Debug().Model(User{}).Where("id = ?", id).Take(&u).Error
	if err != nil {
		return &User{}, err
	}
	if err == gorm.ErrRecordNotFound {
		return &User{}, errors.New("User Not Found")
	}
	return u, err
}

func (u *User) Update(db *gorm.DB, id uint32) (*User, error) {

	db = db.Debug().Model(&User{}).Where("id = ?", id).Take(&User{}).UpdateColumns(
		map[string]interface{}{
			"last_name":  u.LastName,
			"first_name": u.FirstName,
			"badge_id":   u.BadgeID,
			"admin":      u.Admin,
			"update_at":  time.Now(),
		},
	)
	if db.Error != nil {
		return &User{}, db.Error
	}
	// This is the display the updated user
	err := db.Debug().Model(&User{}).Where("id = ?", id).Take(&u).Error
	if err != nil {
		return &User{}, err
	}
	return u, nil
}

func (u *User) Delete(db *gorm.DB, id uint32) (int64, error) {
	db = db.Debug().Model(&User{}).Where("id = ?", id).Take(&User{}).Delete(&User{})

	if db.Error != nil {
		return 0, db.Error
	}
	return db.RowsAffected, nil
}
