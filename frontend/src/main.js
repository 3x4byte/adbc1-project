import './assets/main.css'

import { createApp } from 'vue'
import App from './App.vue'
import router from './router'

import PocketBase from 'pocketbase';


const app = createApp(App)
app.config.globalProperties.$pb = new PocketBase('http://141.46.244.163:8090/')

app.use(router)

app.mount('#app')
