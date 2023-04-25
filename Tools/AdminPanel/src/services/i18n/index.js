/* eslint-disable import/no-anonymous-default-export */
import i18n from "i18next";
import { initReactI18next } from "react-i18next";
import { i18n as data } from "../../resources";

i18n
  .use(initReactI18next) // passes i18n down to react-i18next
  .init({
    resources: { en: data },
    lng: "en",
    interpolation: {
      escapeValue: false, // react already safes from xss
    },
  });

export default i18n;
