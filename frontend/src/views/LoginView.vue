<template>
    <div class="login-container">
      <h1>Login</h1>
      <form @submit.prevent="handleLogin">
        <div>
          <label for="username">Benutzername</label>
          <input type="text" id="username" v-model="username" required />
        </div>
        <div>
          <label for="password">Passwort</label>
          <input type="password" id="password" v-model="password" required />
        </div>
        <button type="submit">Anmelden</button>
      </form>
    </div>
  </template>
  
  <script>
  import { defineComponent } from 'vue';
  
  export default defineComponent({
    name: 'LoginView',
    data() {
      return {
        username: '',
        password: '',
      };
    },
    methods: {
      async handleLogin() {
        await this.$pb.collection("users").authWithPassword(this.username, this.password);

        if (this.$pb.authStore.isValid) {
          this.$router.push('/dashboard'); // Weiterleiten
        } else {
          alert('Ungültige Anmeldedaten');
        }
      },
    },
  });
  </script>
  
  <style scoped>
  .login-container {
    max-width: 400px;
    margin: 50px auto;
    padding: 20px;
    border: 1px solid #ccc;
    border-radius: 8px;
  }
  </style>